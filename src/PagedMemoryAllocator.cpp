#include "PagedMemoryAllocator.h"
#include <iostream>
#include <algorithm>

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

void PagedMemoryAllocator::destroy() {
    std::lock_guard<std::mutex> lock(mtx);
    delete sharedInstance;
    sharedInstance = nullptr;
}

void* PagedMemoryAllocator::allocate(size_t size, int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    if (size == 0) return nullptr;

    size_t pagesNeeded = (size + frameSize - 1) / frameSize;

    size_t freeFrames = 0;
    for (bool occupied : frameTable) if (!occupied) freeFrames++;
    if (freeFrames == 0) return nullptr;

    auto* pageTable = new PageTable();
    pageTable->requestedSize = size;
    pageTable->pid = pid;
    pageTable->entries.resize(pagesNeeded);

    currentAllocatedSize += size;
    activeAllocations.push_back(pageTable);

    handlePageFault(pageTable, 0);

    return static_cast<void*>(pageTable);
}

void PagedMemoryAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    std::lock_guard<std::mutex> lock(mtx);

    auto* pageTable = static_cast<PageTable*>(ptr);

    for (size_t i = 0; i < pageTable->entries.size(); i++) {
        PageEntry& entry = pageTable->entries[i];
        if (entry.isValid && entry.frameNumber >= 0 && entry.frameNumber < (int)totalFrames) {
            size_t f = (size_t)entry.frameNumber;
            frameTable[f] = false;
            frameOwner.erase(f);
            lruManager.remove(f);
        }
    }

    currentAllocatedSize -= pageTable->requestedSize;

    auto it = std::find(activeAllocations.begin(), activeAllocations.end(), pageTable);
    if (it != activeAllocations.end()) activeAllocations.erase(it);
    delete pageTable;
}

uint16_t PagedMemoryAllocator::readWord(PageTable* pt, size_t addr) {
    std::lock_guard<std::mutex> lock(mtx);

    size_t pageIndex = addr / frameSize;
    size_t offset = addr % frameSize;

    int f = handlePageFault(pt, pageIndex);
    if (f < 0) return 0;
    pt->entries[pageIndex].isPinned = true;

    uint8_t lo = physicalMemory[(size_t)f * frameSize + offset];

    uint8_t hi = 0;
    if (offset + 1 < frameSize) {
        hi = physicalMemory[(size_t)f * frameSize + offset + 1];
    }
    else {
        int f2 = handlePageFault(pt, pageIndex + 1);
        if (f2 >= 0) {
            pt->entries[pageIndex + 1].isPinned = true;
            hi = physicalMemory[(size_t)f2 * frameSize + 0];
            pt->entries[pageIndex + 1].isPinned = false;
        }
    }

    pt->entries[pageIndex].isPinned = false;
    return static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8);
}

void PagedMemoryAllocator::writeWord(PageTable* pt, size_t addr, uint16_t value) {
    std::lock_guard<std::mutex> lock(mtx);

    size_t pageIndex = addr / frameSize;
    size_t offset = addr % frameSize;

    int f = handlePageFault(pt, pageIndex);
    if (f < 0) return;
    pt->entries[pageIndex].isPinned = true;

    physicalMemory[(size_t)f * frameSize + offset] = static_cast<uint8_t>(value & 0xFF);
    pt->entries[pageIndex].isDirty = true;

    if (offset + 1 < frameSize) {
        physicalMemory[(size_t)f * frameSize + offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    }
    else {
        int f2 = handlePageFault(pt, pageIndex + 1);
        if (f2 >= 0) {
            pt->entries[pageIndex + 1].isPinned = true;
            physicalMemory[(size_t)f2 * frameSize + 0] = static_cast<uint8_t>((value >> 8) & 0xFF);
            pt->entries[pageIndex + 1].isDirty = true;
            pt->entries[pageIndex + 1].isPinned = false;
        }
    }

    pt->entries[pageIndex].isPinned = false;
}

int PagedMemoryAllocator::handlePageFault(PageTable* pt, size_t pageIndex) {
    PageEntry& entry = pt->entries[pageIndex];

    if (entry.isValid) {
        lruManager.touch((size_t)entry.frameNumber);
        return entry.frameNumber;
    }

    long long frame = -1;
    for (size_t i = 0; i < totalFrames; i++) {
        if (!frameTable[i]) { frame = (long long)i; break; }
    }

    if (frame == -1) {
        long long victim = selectVictim();
        if (victim < 0) return -1;   // all frames pinned
        evictPage((size_t)victim);
        frame = victim;
    }

    frameTable[(size_t)frame] = true;
    entry.frameNumber = (int)frame;
    entry.isValid = true;
    entry.isDirty = false;

    if (entry.backingSlot < 0) entry.backingSlot = nextBackingSlot++;

    frameOwner[(size_t)frame] = { pt, pageIndex };
    lruManager.touch((size_t)frame);

    uint8_t* frameStart = &physicalMemory[(size_t)frame * frameSize];
    backingStore.read_page((int)entry.backingSlot, frameStart);
    numPagedIn++;

    return (int)frame;
}

long long PagedMemoryAllocator::selectVictim() {
    std::vector<size_t> skipped;
    long long chosen = -1;

    while (true) {
        long long lru = lruManager.removeLRU();
        if (lru < 0) break;

        auto it = frameOwner.find((size_t)lru);
        if (it == frameOwner.end() || it->second.first == nullptr) continue;

        PageTable* pt = it->second.first;
        size_t pageIdx = it->second.second;
        if (!pt->entries[pageIdx].isPinned) {
            chosen = lru;
            break;
        }
        skipped.push_back((size_t)lru);
    }

    for (size_t f : skipped) lruManager.requeueAsMRU(f);
    return chosen;
}

void PagedMemoryAllocator::evictPage(size_t frameIndex) {
    auto it = frameOwner.find(frameIndex);
    if (it == frameOwner.end()) return;

    PageTable* pt = it->second.first;
    size_t pageIndex = it->second.second;
    PageEntry& entry = pt->entries[pageIndex];

    if (entry.isDirty) {
        if (entry.backingSlot < 0) entry.backingSlot = nextBackingSlot++;
        uint8_t* frameStart = &physicalMemory[frameIndex * frameSize];
        backingStore.write_page((int)entry.backingSlot, frameStart);
        numPagedOut++;
    }

    lruManager.remove(frameIndex);

    entry.isValid = false;
    entry.frameNumber = -1;
    frameTable[frameIndex] = false;
    frameOwner.erase(it);
}

size_t PagedMemoryAllocator::getResidentMemory(PageTable* pt) const {
    std::lock_guard<std::mutex> lock(mtx);
    if (!pt) return 0;
    size_t resident = 0;
    for (const auto& e : pt->entries) if (e.isValid) resident++;
    return resident * frameSize;
}