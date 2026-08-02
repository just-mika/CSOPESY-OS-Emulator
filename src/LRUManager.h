#pragma once
#include <list>
#include <unordered_map>

class LRUManager {
public:
    LRUManager(size_t maxFrames = 0) : maxFrames(maxFrames) {}

    void accessPage(int pageId) {
        // Remove from its current position if already tracked
        auto it = pageMap.find(pageId);
        if (it != pageMap.end()) {
            lruList.erase(it->second);
        }
        // Push to front as MRU
        lruList.push_front(pageId);
        pageMap[pageId] = lruList.begin();
    }

    int removeFrame() {
        if (lruList.empty()) return -1;
        int lruFrameId = lruList.back();
        lruList.pop_back();
        pageMap.erase(lruFrameId);
        return lruFrameId;
    }

    void removePage(int pageId) {
        auto it = pageMap.find(pageId);
        if (it != pageMap.end()) {
            lruList.erase(it->second);
            pageMap.erase(it);
        }
    }

private:
    size_t maxFrames;
    std::list<int> lruList;                                  // access order (front = MRU)
    std::unordered_map<int, std::list<int>::iterator> pageMap;
};