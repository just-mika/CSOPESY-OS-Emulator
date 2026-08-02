#include "PagedMemoryAllocator.h"
#include <iostream>
#include <list>
#include <unordered_map>
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

    size_t framesNeeded = (size + frameSize - 1) / frameSize;

    size_t freeFrames = 0;
    for (bool occupied : frameTable) if (!occupied) freeFrames++;
    if (freeFrames < framesNeeded) {
        return nullptr;      
    }

    auto* pageTable = new PageTable();
    pageTable->requestedSize = size;
    pageTable->pid = pid;
    pageTable->entries.resize(framesNeeded);

    size_t assigned = 0;
    for (size_t i = 0; i < totalFrames && assigned < framesNeeded; i++) {
        if (!frameTable[i]) {
            frameTable[i] = true;
            PageEntry& e = pageTable->entries[assigned];
            e.frameNumber = (int)i;
            e.isValid = true;
            e.isDirty = false;
            frameOwner[i] = { pageTable, assigned };
            int pageId = pid * 1000 + (int)assigned;
            lruManager.accessPage(pageId);
            uint8_t* frameStart = &physicalMemory[i * frameSize];
            backingStore.read_page(pageId, frameStart);
            assigned++;
        }
    }

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
            if (entry.frameNumber >= 0 && entry.frameNumber < (int)totalFrames) {
                frameTable[entry.frameNumber] = false;
                frameOwner.erase((size_t)entry.frameNumber);
            }
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

uint16_t PagedMemoryAllocator::readWord(PageTable* pt, size_t addr) {
    std::lock_guard<std::mutex> lock(mtx);

    size_t pageIndex = addr / frameSize;
    size_t offset = addr % frameSize;

    handlePageFault(pt, pageIndex);
    pt->entries[pageIndex].isPinned = true;   // protect from the second fault's victim search

    int f = pt->entries[pageIndex].frameNumber;
    uint8_t lo = physicalMemory[(size_t)f * frameSize + offset];

    uint8_t hi;
    if (offset + 1 < frameSize) {
        hi = physicalMemory[(size_t)f * frameSize + offset + 1];
    }
    else {
        // Word straddles a page boundary.
        handlePageFault(pt, pageIndex + 1);
        int f2 = pt->entries[pageIndex + 1].frameNumber;
        hi = physicalMemory[(size_t)f2 * frameSize + 0];
    }

    pt->entries[pageIndex].isPinned = false;
    return static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8);
}

void PagedMemoryAllocator::writeWord(PageTable* pt, size_t addr, uint16_t value) {
    std::lock_guard<std::mutex> lock(mtx);

    size_t pageIndex = addr / frameSize;
    size_t offset = addr % frameSize;

    handlePageFault(pt, pageIndex);
    pt->entries[pageIndex].isPinned = true;

    int f = pt->entries[pageIndex].frameNumber;
    physicalMemory[(size_t)f * frameSize + offset] = static_cast<uint8_t>(value & 0xFF);
    pt->entries[pageIndex].isDirty = true;

    if (offset + 1 < frameSize) {
        physicalMemory[(size_t)f * frameSize + offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    }
    else {
        handlePageFault(pt, pageIndex + 1);
        int f2 = pt->entries[pageIndex + 1].frameNumber;
        physicalMemory[(size_t)f2 * frameSize + 0] = static_cast<uint8_t>((value >> 8) & 0xFF);
        pt->entries[pageIndex + 1].isDirty = true;
    }

    pt->entries[pageIndex].isPinned = false;
}

void PagedMemoryAllocator::handlePageFault(PageTable* pt, size_t pageIndex) {
    PageEntry& entry = pt->entries[pageIndex];
    if (entry.isValid) {
        // Already resident: just mark it most-recently-used.
        int pageId = pt->pid * 1000 + (int)pageIndex;
        lruManager.accessPage(pageId);
        return;
    }

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
    frameOwner[(size_t)freeFrame] = { pt, pageIndex };
    lruManager.accessPage(pageId);

    uint8_t* frameStart = &physicalMemory[(size_t)freeFrame * frameSize];
    backingStore.read_page(pageId, frameStart);
    numPagedIn++;
}

size_t PagedMemoryAllocator::selectVictim() {
    size_t attempts = 0;
    size_t maxAttempts = totalFrames > 0 ? totalFrames * 2 : 100;
    std::vector<int> requeue;
    while (attempts < maxAttempts) {
        int pageId = lruManager.removeFrame();
        if (pageId == -1) break;

        bool matched = false;
        for (auto& [frameIdx, owner] : frameOwner) {
            if (owner.first == nullptr) continue;

            int ownerPageId = owner.first->pid * 1000 + (int)owner.second;
            if (ownerPageId == pageId) {
                matched = true;
                bool pinned = owner.first->entries[owner.second].isPinned;
                if (!pinned) {
                    for (int rid : requeue) lruManager.accessPage(rid);
                    return frameIdx;
                }
                break;
            }
        }
        if (matched) requeue.push_back(pageId);
        attempts++;
    }

    for (int rid : requeue) lruManager.accessPage(rid);

    for (auto& [frameIdx, owner] : frameOwner) {
        if (owner.first != nullptr && !owner.first->entries[owner.second].isPinned) {
            return frameIdx;
        }
    }
    for (auto& [frameIdx, owner] : frameOwner) {
        if (owner.first != nullptr) return frameIdx;
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

    int pageId = pt->pid * 1000 + (int)pageIndex;
    lruManager.removePage(pageId);

    entry.isValid = false;
    entry.frameNumber = -1;
    frameTable[frameIndex] = false;
    frameOwner.erase(it);
}