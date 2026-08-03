#include "BackingStore.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstring>

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
    if (!file.is_open()) return false;

    if (writtenSlots.find(pageId) == writtenSlots.end()) {
        std::memset(buffer, 0, pageSize);
        return true;
    }

    size_t recordWidth = pageSize * 2 + 1;
    std::streampos offset = (std::streampos)pageId * recordWidth;
    file.seekg(offset);

    std::string hexLine(pageSize * 2, '0');
    file.read(&hexLine[0], pageSize * 2);

    if (!file.good()) {
        file.clear();
        std::memset(buffer, 0, pageSize);
        return true;
    }

    uint8_t* out = static_cast<uint8_t*>(buffer);
    for (size_t i = 0; i < pageSize; i++) {
        try {
            out[i] = (uint8_t)std::stoi(hexLine.substr(i * 2, 2), nullptr, 16);
        }
        catch (const std::exception& e) {
            std::cerr << "BAD HEX at pageId=" << pageId << " byte=" << i
                << " raw=\"" << hexLine.substr(i * 2, 2) << "\"\n";
            std::memset(buffer, 0, pageSize);
            return true;
        }
    }
    return true;
}


bool BackingStore::write_page(int pageId, const void* buffer) {
    if (!file.is_open()) {
        return false;
    }

    size_t recordWidth = pageSize * 2 + 1;
    std::streampos offset = (std::streampos)pageId * recordWidth;
    file.seekp(offset);

    const uint8_t* in = static_cast<const uint8_t*>(buffer);
    std::ostringstream oss;
    for (size_t i = 0; i < pageSize; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)in[i];
    }
    oss << '\n';

    file << oss.str();
    file.flush();

    bool success = file.good();
    if (success) writtenSlots.insert(pageId);
    return success;
}