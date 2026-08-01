
#include <list>
#include <unordered_map>

class LRUManager {
public:
    LRUManager(size_t maxFrames = 0) : maxFrames(maxFrames) {}
    void accessPage(int pageId) {
        // Remove frame from its current position if it's already in pageMap
        if (pageMap.find(pageId) != pageMap.end()) {
            lruList.erase(pageMap[pageId]);
        }

        // Push frame to front as MRU
        lruList.push_front(pageId);
        pageMap[pageId] = lruList.begin();
    }
    int removeFrame() {
        if (lruList.empty()) return -1; // Always guaranteed to return something, but just in case : <>

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
    std::list<int> lruList; // Tracks the order of access
    std::unordered_map<int, std::list<int>::iterator> pageMap; // Keeps all pages in RAM
};
