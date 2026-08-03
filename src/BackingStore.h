#pragma once
#include <string>
#include <fstream>
#include <unordered_set>

class BackingStore {
public:
	BackingStore(const std::string& filename, size_t page_size);
	~BackingStore();

	bool read_page(int pageId, void* buffer);
	bool write_page(int pageId, const void* buffer);

private:
	std::fstream file;
	size_t pageSize;
	std::unordered_set<int> writtenSlots;;
};
