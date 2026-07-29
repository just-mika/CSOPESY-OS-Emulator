#include "BackingStore.h"
#include <iostream>

BackingStore::BackingStore(const std::string& fileName, size_t pageSize) : pageSize(pageSize) {
    // Creates a new file if it doesn't exist or clears any contents if it exists
    file.open(fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Fatal Error: Could not create backing store file!" << std::endl;
    }
}

BackingStore::~BackingStore() {
    if (file.is_open()) {
        file.close();
    }
}
bool BackingStore::read_page(int pageId, void* buffer) {
    if (!file.is_open())
        return false;

    std::streampos offset = pageId * pageSize;
    file.seekg(offset);
    file.read(static_cast<char*>(buffer), pageSize);

    return file.good();
}

bool BackingStore::write_page(int pageId, const void* buffer) {
    if (!file.is_open())
        return false;

    std::streampos offset = pageId * pageSize;
    file.seekp(offset);
    file.write(static_cast<const char*>(buffer), pageSize);
    file.flush();
    
    return file.good();
}