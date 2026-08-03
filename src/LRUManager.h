#pragma once
#include <list>
#include <unordered_map>

// Keyed by frame index. Not internally synchronized; caller holds the allocator mutex.
class LRUManager {
public:
    LRUManager(size_t maxFrames = 0) : maxFrames(maxFrames) {}

    void touch(size_t frameIndex) {
        auto it = frameMap.find(frameIndex);
        if (it != frameMap.end()) order.erase(it->second);
        order.push_front(frameIndex);
        frameMap[frameIndex] = order.begin();
    }

    long long removeLRU() {
        if (order.empty()) return -1;
        size_t lru = order.back();
        order.pop_back();
        frameMap.erase(lru);
        return static_cast<long long>(lru);
    }

    void requeueAsMRU(size_t frameIndex) { touch(frameIndex); }

    void remove(size_t frameIndex) {
        auto it = frameMap.find(frameIndex);
        if (it != frameMap.end()) {
            order.erase(it->second);
            frameMap.erase(it);
        }
    }

    bool empty() const { return order.empty(); }

private:
    size_t maxFrames;
    std::list<size_t> order;
    std::unordered_map<size_t, std::list<size_t>::iterator> frameMap;
};