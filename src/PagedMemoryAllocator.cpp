#include "PagedMemoryAllocator.h"
#include <list>
#include <unordered_map>

PagedMemoryAllocator* PagedMemoryAllocator::sharedInstance = nullptr;
std::mutex PagedMemoryAllocator::mtx;

PagedMemoryAllocator::PagedMemoryAllocator(size_t totalMemory, size_t frameSize, const std::string& backingStoreFile)
    : frameSize(frameSize), backingStore(backingStoreFile, frameSize) {
    memoryAllocatorType = PAGING;
    maximumSize = totalMemory;
    currentAllocatedSize = 0;
    totalFrames = totalMemory / frameSize;
    frameTable.resize(totalFrames, false);
    physicalMemory.resize(totalFrames * frameSize, 0);
    lruManager = LRUManager(totalFrames);
}

void PagedMemoryAllocator::init(size_t totalMemory, size_t frameSize, const std::string& backingStoreFile) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!sharedInstance) {
        sharedInstance = new PagedMemoryAllocator(totalMemory, frameSize, backingStoreFile);
    }
}
PagedMemoryAllocator* PagedMemoryAllocator::getInstance() {
    return sharedInstance;
}

void* PagedMemoryAllocator::allocate(size_t size, int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    if (size == 0 || size > (maximumSize - currentAllocatedSize)) {
        return nullptr;
    }

    size_t framesNeeded = (size + frameSize - 1) / frameSize;

    auto* pageTable = new PageTable();
    pageTable->requestedSize = size;
    pageTable->pid = pid;
    pageTable->entries.resize(framesNeeded);

    currentAllocatedSize += size;
    activeAllocations.push_back(pageTable);
    return static_cast<void*>(pageTable);
}

void PagedMemoryAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    std::lock_guard<std::mutex> lock(mtx);

    auto* pageTable = static_cast<PageTable*>(ptr);

    for (size_t i = 0; i < pageTable->entries.size(); i++) {
        PageEntry& entry = pageTable->entries[i];
        if (entry.isValid) {
            frameTable[entry.frameNumber] = false;
            frameOwner.erase(entry.frameNumber);
            int pageId = pageTable->pid * 1000 + (int)i;
            lruManager.removePage(pageId);
        }
    }

    currentAllocatedSize -= pageTable->requestedSize;

    auto it = std::find(activeAllocations.begin(), activeAllocations.end(), pageTable);
    if (it != activeAllocations.end()) {
        activeAllocations.erase(it);
    }
    delete pageTable;
}

void PagedMemoryAllocator::handlePageFault(PageTable* pt, size_t pageIndex) {
    PageEntry& entry = pt->entries[pageIndex];
    if (entry.isValid) return;

    int freeFrame = -1;
    for (size_t i = 0; i < totalFrames; i++) {
        if (!frameTable[i]) { freeFrame = (int)i; break; }
    }

    if (freeFrame == -1) {
        freeFrame = (int)selectVictim();
        evictPage((size_t)freeFrame);
    }

    frameTable[freeFrame] = true;
    entry.frameNumber = freeFrame;
    entry.isValid = true;
    entry.isDirty = false;

    int pageId = pt->pid * 1000 + (int)pageIndex;
    frameOwner[freeFrame] = { pt, pageIndex };
    lruManager.accessPage(pageId);

    uint8_t* frameStart = &physicalMemory[freeFrame * frameSize];
    backingStore.read_page(pageId, frameStart);
    numPagedIn++;
}

size_t PagedMemoryAllocator::selectVictim() {
    int pageId = lruManager.removeFrame();
    for (auto& [frameIdx, owner] : frameOwner) {
        int ownerPageId = owner.first->pid * 1000 + (int)owner.second;
        if (ownerPageId == pageId) return frameIdx;
    }
    return 0;
}

void PagedMemoryAllocator::evictPage(size_t frameIndex) {
    auto it = frameOwner.find(frameIndex);
    if (it == frameOwner.end()) return;

    PageTable* pt = it->second.first;
    size_t pageIndex = it->second.second;
    PageEntry& entry = pt->entries[pageIndex];

    if (entry.isDirty) {
        int pageId = pt->pid * 1000 + (int)pageIndex;
        uint8_t* frameStart = &physicalMemory[frameIndex * frameSize];
        backingStore.write_page(pageId, frameStart);
        numPagedOut++;
    }

    entry.isValid = false;
    entry.frameNumber = -1;
    frameTable[frameIndex] = false;
    frameOwner.erase(it);
}