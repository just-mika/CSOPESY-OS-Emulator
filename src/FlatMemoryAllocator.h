#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <mutex>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    static FlatMemoryAllocator* getInstance();
    static void init(size_t maximumSize);

    FlatMemoryAllocator(const FlatMemoryAllocator&) = delete;
    FlatMemoryAllocator& operator=(const FlatMemoryAllocator&) = delete;

    ~FlatMemoryAllocator() = default;

    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    std::string visualizeMemory() override;

private:
    FlatMemoryAllocator(size_t maximumSize);

    static FlatMemoryAllocator* sharedInstance;
    static std::mutex mtx;

    size_t maximumSize;
    size_t allocatedSize;
    std::vector<char> memory;
    std::vector<bool> allocationMap;
    std::unordered_map<size_t, size_t> allocationSizes;

    bool canAllocateAt(size_t index, size_t size) const;
    void allocateAt(size_t index, size_t size);
    void deallocateAt(size_t index);
};

