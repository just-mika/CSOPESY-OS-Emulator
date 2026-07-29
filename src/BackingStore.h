#pragma once
#include <string>
#include <fstream>

class BackingStore {
public:
	BackingStore(const std::string& filename, size_t page_size);
	~BackingStore();

	bool read_page(int pageId, void* buffer);
	bool write_page(int pageId, const void* buffer);

private:
	std::fstream file;
	size_t pageSize;
};