#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <iostream>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
	FlatMemoryAllocator(size_t maximumSize) : maximumSize(maximumSize), allocatedSize(0) {
		memory.resize(maximumSize);
		initializeMemory();
	}

	~FlatMemoryAllocator() {
		memory.clear();
	}

	void* allocate(size_t size) override;
	void deallocate(void* ptr) override;
	std::string visualizeMemory() override;

private:
		size_t maximumSize;									// Maximum size of the flat memory block
		size_t allocatedSize;								// Current allocated size
		std::vector<char> memory;							// The flat memory block visualizer
		std::vector<bool> allocationMap;					// Map to track allocated blocks
		std::unordered_map<size_t, size_t> allocationSizes; // Tracks index to size to prevent leaks during deallocation
		void initializeMemory();
		bool canAllocateAt(size_t index, size_t size) const;
		void allocateAt(size_t index, size_t size);
		void deallocateAt(size_t index);

};

