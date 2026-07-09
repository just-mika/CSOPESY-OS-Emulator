#include "FlatMemoryAllocator.h"
#include <algorithm>

void* FlatMemoryAllocator::allocate(size_t size)
{
	// First-fit: Scan the allocation map for a free block of the requested size
    for (size_t i = 0; i < maximumSize - size + 1; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
			return &memory[i]; // Return the pointer to the allocated block
        }
	}

	// No available block found, return nullptr
    return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr)
{
	if (!ptr) return;

	// Find the index of the memory block to deallocate
    size_t index = static_cast<char*>(ptr) - &memory[0];
    if (index < maximumSize && allocationMap[index]) {
        deallocateAt(index);
	}
}

std::string FlatMemoryAllocator::visualizeMemory()
{
    return std::string(memory.begin(), memory.end());
}

void FlatMemoryAllocator::initializeMemory()
{
	std::fill(memory.begin(), memory.end(), '.');    // '.' represents free memory
	allocationMap.assign(maximumSize, false);		 // Mark all blocks as free
	allocationSizes.clear();
	allocatedSize = 0;
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const
{
	if (index + size > maximumSize) return false;

	// Verify that the requested block does not overlap with any already allocated blocks
	for (size_t i = index; i < index + size; ++i) {
		if (allocationMap[i]) {
			return false; // Found an overlapping allocated byte
		}
	}
	return true;
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size)
{
	std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, true); // Mark the block as allocated
	std::fill(memory.begin() + index, memory.begin() + index + size, '#');				  // '#' represents allocated memory
	allocationSizes[index] = size;
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index)
{
	// Look up the size of the allocated block at this index
	size_t size = allocationSizes[index];

	// Mark the whole memory block range as free
	std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, false);
	std::fill(memory.begin() + index, memory.begin() + index + size, '.');

	allocatedSize -= size;
	allocationSizes.erase(index); // Clean up the size tracker record
}
