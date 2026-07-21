#include "PagedMemoryAllocator.h"

PagedMemoryAllocator* PagedMemoryAllocator::sharedInstance = nullptr;
std::mutex PagedMemoryAllocator::mtx;

PagedMemoryAllocator::PagedMemoryAllocator(size_t totalMemory, size_t frameSize) : frameSize(frameSize) {
    memoryAllocatorType = PAGING;
    maximumSize = totalMemory;
    currentAllocatedSize = 0;
    totalFrames = totalMemory / frameSize;
    frameTable.resize(totalFrames, false);
}

void PagedMemoryAllocator::init(size_t totalMemory, size_t frameSize) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!sharedInstance) {
        sharedInstance = new PagedMemoryAllocator(totalMemory, frameSize);
    }
}
PagedMemoryAllocator* PagedMemoryAllocator::getInstance() {
    return sharedInstance;
}
void* PagedMemoryAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mtx); 
    if (size == 0 || size > (maximumSize - currentAllocatedSize)) {
        return nullptr;
    }
    size_t framesNeeded = (size + frameSize - 1) / frameSize;
    size_t freeFrames = 0;
    for (bool isOccupied : frameTable) {
        if (!isOccupied) freeFrames++;
    }

    if (freeFrames < framesNeeded) {
        return nullptr;
    }

    auto* pageTable = new PageTable();
    pageTable->requestedSize = size;

    for (size_t i = 0; i < totalFrames && pageTable->frameNumbers.size() < framesNeeded; i++) {
        if (!frameTable[i]) {
            frameTable[i] = true;
            pageTable->frameNumbers.push_back(i);
        }
    }

    currentAllocatedSize += framesNeeded * frameSize;
    activeAllocations.push_back(pageTable);
    return static_cast<void*>(pageTable);
}

void PagedMemoryAllocator::deallocate(void* ptr) {
    if (!ptr) return;

    std::lock_guard<std::mutex> lock(mtx);

    auto* pageTable = static_cast<PageTable*>(ptr);
    for (size_t frameIdx : pageTable->frameNumbers) {
        if (frameIdx < totalFrames) {
            frameTable[frameIdx] = false;
        }
    }
    currentAllocatedSize -= pageTable->frameNumbers.size() * frameSize;

    auto it = std::find(activeAllocations.begin(), activeAllocations.end(), pageTable);
    if (it != activeAllocations.end()) {
        activeAllocations.erase(it);
    }
    delete pageTable;
}