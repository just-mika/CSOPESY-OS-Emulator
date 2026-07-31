#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cmath>
#include "IMemoryAllocator.h"


class PagedMemoryAllocator : public IMemoryAllocator {
public:
    struct PageTable {
        size_t requestedSize;
        std::vector<size_t> frameNumbers;
    };
    static void init(size_t totalMemory, size_t frameSize);
    static PagedMemoryAllocator* getInstance();
    ~PagedMemoryAllocator() = default;
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    static void destroy();

    std::string visualizeMemory() override {
        std::ostringstream oss;
        oss << "=== Paged Memory Allocator ===\n";
        oss << "Total Memory: " << maximumSize << " KB | Frame Size: " << frameSize << " KB\n";
        oss << "Total Frames: " << totalFrames << " | Allocated Frames: "
            << (currentAllocatedSize / frameSize) << " | Free Frames: "
            << (totalFrames - (currentAllocatedSize / frameSize)) << "\n\n";

        oss << "Frame Status:\n";
        for (size_t i = 0; i < totalFrames; ++i) {
            oss << "Frame " << i << ": " << (frameTable[i] ? "[ALLOCATED]" : "[FREE]") << "\n";
        }

        return oss.str();
    }


private:
    PagedMemoryAllocator(size_t totalMemory, size_t frameSize);
    static PagedMemoryAllocator* sharedInstance; 
    static std::mutex mtx;
    size_t frameSize;
    size_t totalFrames;
    std::vector<bool> frameTable;
    std::vector<PageTable*> activeAllocations;
};