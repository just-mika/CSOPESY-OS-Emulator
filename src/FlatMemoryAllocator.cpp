#include "FlatMemoryAllocator.h"
#include <algorithm>

FlatMemoryAllocator* FlatMemoryAllocator::sharedInstance = nullptr;
std::mutex FlatMemoryAllocator::mtx;

FlatMemoryAllocator::FlatMemoryAllocator(size_t size)
    : maximumSize(size), allocatedSize(0) {
    memory.resize(size, '.');
    allocationMap.assign(size, false);
}

void FlatMemoryAllocator::init(size_t size) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!sharedInstance) {
        sharedInstance = new FlatMemoryAllocator(size);
    }
    sharedInstance->memory.resize(size);
}

FlatMemoryAllocator* FlatMemoryAllocator::getInstance() {
    return sharedInstance;
}

void* FlatMemoryAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mtx);

    for (size_t i = 0; i <= maximumSize - size; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
            return &memory[i];
        }
    }
    return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    std::lock_guard<std::mutex> lock(mtx);

    size_t index = static_cast<char*>(ptr) - &memory[0];
    if (index < maximumSize && allocationMap[index]) {
        deallocateAt(index);
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    if (index + size > maximumSize) return false;
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap[i]) return false;
    }
    return true;
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, true);
    std::fill(memory.begin() + index, memory.begin() + index + size, '#');
    allocationSizes[index] = size;
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
    size_t size = allocationSizes[index];
    std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, false);
    std::fill(memory.begin() + index, memory.begin() + index + size, '.');
    allocatedSize -= size;
    allocationSizes.erase(index);
}

std::string FlatMemoryAllocator::visualizeMemory() {
    std::lock_guard<std::mutex> lock(mtx);
    return std::string(memory.begin(), memory.end());
}