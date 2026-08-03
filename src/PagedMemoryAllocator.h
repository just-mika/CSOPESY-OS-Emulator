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
    bool isPinned = false;
    long long backingSlot = -1;
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

    size_t getUsedMemory() const {
        std::lock_guard<std::mutex> lock(mtx);
        size_t allocatedFrames = 0;
        for (bool occupied : frameTable) if (occupied) allocatedFrames++;
        return allocatedFrames * frameSize;
    }
    size_t getTotalMemory() const { return maximumSize; }
    size_t getFreeMemory() const { return maximumSize - getUsedMemory(); }

    std::string visualizeMemory() override {
        std::lock_guard<std::mutex> lock(mtx);
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

    uint16_t readWord(PageTable* pt, size_t addr);
    void writeWord(PageTable* pt, size_t addr, uint16_t value);

    size_t getFrameSize() const { return frameSize; }

    int ensurePageResident(PageTable* pt, size_t pageIndex) {
        std::lock_guard<std::mutex> lock(mtx);
        return handlePageFault(pt, pageIndex);   // -1 on failure, frame number on success
    }

    uint8_t* getFramePointer(size_t frameIndex) {
        return &physicalMemory[frameIndex * frameSize];
    }
    size_t getResidentMemory(PageTable* pt) const;

private:
    PagedMemoryAllocator(size_t totalMemory, size_t frameSize, const std::string& backingStoreFile);
    static PagedMemoryAllocator* sharedInstance;
    static std::mutex mtx;
    size_t frameSize;
    size_t totalFrames;
    std::vector<bool> frameTable;
    std::vector<PageTable*> activeAllocations;

    std::vector<uint8_t> physicalMemory;
    LRUManager lruManager;
    size_t numPagedIn = 0;
    size_t numPagedOut = 0;
    std::unordered_map<size_t, FrameOwner> frameOwner;
    BackingStore backingStore;
    long long nextBackingSlot = 0;

    // All assume mtx is held. handlePageFault returns the resident frame, or -1
    // if none could be freed. selectVictim returns an evictable frame, or -1.
    int handlePageFault(PageTable* pt, size_t pageIndex);
    long long selectVictim();
    void evictPage(size_t frameIndex);
};