#include "PagedMemoryAllocator.h"
#include <list>
#include <unordered_map>
#include <algorithm>

class LRUManager {
public:
    LRUManager(size_t maxFrames) : maxFrames(maxFrames) {}

    void accessPage(size_t frameId) {
        std::lock_guard<std::mutex> lock(lruMtx);
        if (pageMap.find(frameId) != pageMap.end()) {
            lruList.erase(pageMap[frameId]);
        }
        lruList.push_front(frameId);
        pageMap[frameId] = lruList.begin();
        
    }

    size_t evictFrame() {
        std::lock_guard<std::mutex> lock(lruMtx);
        if (lruList.empty()) return -1;

        size_t lruFrameId = lruList.back();
        lruList.pop_back();
        pageMap.erase(lruFrameId);
        return lruFrameId;
    }

    void removeFrame(size_t frameId) {
        std::lock_guard<std::mutex> lock(lruMtx);
        if (pageMap.find(frameId) != pageMap.end()) {
            lruList.erase(pageMap[frameId]);
            pageMap.erase(frameId);
        }
    }
    int getTotalFramesInRam() {
        std::lock_guard<std::mutex> lock(lruMtx);
        return lruList.size();
    }

private:
    std::mutex lruMtx;
    size_t maxFrames;
    std::list<size_t> lruList;
    std::unordered_map<size_t, std::list<size_t>::iterator> pageMap;
};


PagedMemoryAllocator* PagedMemoryAllocator::sharedInstance = nullptr;
std::mutex PagedMemoryAllocator::mtx;

PagedMemoryAllocator::PagedMemoryAllocator(size_t totalMemory, size_t frameSize) : frameSize(frameSize) {
    memoryAllocatorType = PAGING;
    maximumSize = totalMemory;
    currentAllocatedSize = 0;
    totalFrames = totalMemory / frameSize;
    frameTable.resize(totalFrames, false);
    lruManager = new LRUManager(totalFrames);
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
            lruManager->accessPage(i);
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

            lruManager->removeFrame(frameIdx);
        }
    }

    currentAllocatedSize -= pageTable->frameNumbers.size() * frameSize;

    auto it = std::find(activeAllocations.begin(), activeAllocations.end(), pageTable);
    if (it != activeAllocations.end()) {
        activeAllocations.erase(it);
    }

    delete pageTable;
}