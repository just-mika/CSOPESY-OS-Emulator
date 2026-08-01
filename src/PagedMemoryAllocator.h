#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cmath>

#include <unordered_map>
#include <utility>

#include "BackingStore.h"
#include "IMemoryAllocator.h"
#include "LRUManager.h"

struct PageEntry {
    int frameNumber = -1;
    bool isValid = false;
    bool isDirty = false;
};

struct PageTable {
    size_t requestedSize;
    std::vector<PageEntry> entries;
    int pid;
};

typedef std::pair<PageTable*, size_t> FrameOwner;

class PagedMemoryAllocator : public IMemoryAllocator {
public:
    static void init(size_t totalMemory, size_t frameSize, const std::string& backingStoreFile);
    static PagedMemoryAllocator* getInstance();
    ~PagedMemoryAllocator() = default;
    void* allocate(size_t size, int pid) override;
    void deallocate(void* ptr) override;
    static void destroy();

    size_t getNumPagedIn() const { return numPagedIn; }
    size_t getNumPagedOut() const { return numPagedOut; }

    std::string visualizeMemory() override {
        std::ostringstream oss;
        size_t allocatedFrames = 0;
        for (bool occupied : frameTable) if (occupied) allocatedFrames++;

        oss << "=== Paged Memory Allocator ===\n";
        oss << "Total Memory: " << maximumSize << " KB | Frame Size: " << frameSize << " KB\n";
        oss << "Total Frames: " << totalFrames << " | Allocated Frames: " << allocatedFrames
            << " | Free Frames: " << (totalFrames - allocatedFrames) << "\n\n";

        oss << "Frame Status:\n";
        for (size_t i = 0; i < totalFrames; ++i) {
            oss << "Frame " << i << ": " << (frameTable[i] ? "[ALLOCATED]" : "[FREE]") << "\n";
        }
        return oss.str();
    }


private:
    PagedMemoryAllocator(size_t totalMemory, size_t frameSize, const std::string& backingStoreFile);
    static PagedMemoryAllocator* sharedInstance; 
    static std::mutex mtx;
    size_t frameSize;
    size_t totalFrames;
    std::vector<bool> frameTable;
    std::vector<PageTable*> activeAllocations;

    //for demand paging
    std::vector<uint8_t> physicalMemory;
    LRUManager lruManager;
    size_t numPagedIn = 0;
    size_t numPagedOut = 0;
    std::unordered_map<size_t, FrameOwner> frameOwner;
    BackingStore backingStore;

    void handlePageFault(PageTable* pt, size_t pageIndex);
    size_t selectVictim();
    void evictPage(size_t frameIndex);
};