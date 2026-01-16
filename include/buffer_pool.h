#include <cstdint>       // int32_t, uint64_t
#include <cstring>       // memset, memcpy, strcmp
#include <string>        // std::string
#include <vector>        // std::vector
#include <list>          // std::list
#include <unordered_map> // std::unordered_map
#include <fstream>       // std::fstream
#include <stdexcept>     // std::runtime_error
#include <iostream>      // std::cout
#include "common.h"
#include "page.h"
#include "disk_manager.h"
namespace minidb
{
    class buffer_pool
    {
    public:
        buffer_pool(int frames, DiskManager *dm);

        /// @brief Gets page from cache or disk (pins it)
        /// @param page_id Page ID to retrieve
        /// @return Page with ID page_id
        Page *FetchPage(page_id_t page_id);

        /// @brief Creates new page
        /// @param page_id page ID to assign
        /// @return Created Page
        Page *NewPage(page_id_t *page_id);

        /// @brief Decrements the page count and sets dirty flag
        /// @param page_id Page ID to decrement pin
        /// @param isDirty Set dirty
        void UnpinPage(page_id_t page_id, bool isDirty);

        /// @brief Manually flush page
        /// @param page_id Page to flush
        void FlushPage(page_id_t page_id);

        /// @brief Manuall flush all pages
        void FlushAllPages();

    private:
        /// @brief Contiguous memory-list of pages. This is cache
        std::vector<Page> pages_;

        /// @brief Lookup table to see if pages are in cache. Map pageID to frameID
        std::unordered_map<page_id_t, frame_id_t> page_table_;

        /// @brief Tracks unused frames, or free frames
        std::list<frame_id_t> free_list;

        /// @brief Tracks Least Recently Used, end node is candidate for removal
        std::list<frame_id_t> lru_list_;

        /// @brief Maps frame ID to position in LRU list
        std::unordered_map<frame_id_t, std::list<frame_id_t>::iterator> lru_map_;

        /// @brief Pointer to a disk manager it will use
        DiskManager *disk_manager_;

        /// @brief How many frames in buffer pool
        size_t pool_size_;

        /// @brief Eviction logic for FetchPage and NewPage
        /// @param frame_id frame to evict
        /// @return True if found, false if all pinned
        bool FindVictim(frame_id_t *frame_id_ptr)
        {
            for (int i = lru_list_.size() - 1; i >= 0; i--)
            {
                if (pages_[i].GetPinCount() == 0) // No one is using this page
                {
                    *frame_id_ptr = i;
                    return true;
                }
            }
            return false;
        }

        /// @brief Add frame to LRU to front
        /// @param frame_id Frame to add
        /// @return If success
        bool AddToLRU(frame_id_t frame_id)
        {
            lru_list_.push_front(frame_id);
            lru_map_[frame_id] = lru_list_.begin();
            return true;
        }

        /// @brief Frame to remove from LRU
        /// @param frame_id frame to remove
        /// @return If success
        bool RemoveFromLRU(frame_id_t frame_id)
        {
            auto lru_position = lru_map_.find(frame_id);
            if (lru_position != lru_map_.end()) // if lru_position was found
            {
                lru_list_.erase(lru_position->second);
                lru_map_.erase(frame_id);
                return true;
            }
            return false;
        }

        /// @brief Move frame ID to front of LRU (Recently used)
        /// @param frame_id Frame that was used
        /// @return If success
        bool UpdateLRU(frame_id_t frame_id)
        {
            auto lru_position = lru_map_.find(frame_id);
            if (lru_position != lru_map_.end()) // if lru_position was found
            {
                lru_list_.erase(lru_position->second);
                lru_map_.erase(frame_id);

                lru_list_.push_front(frame_id);
                lru_map_[frame_id] = lru_list_.begin();
                return true;
            }
            return false;
        }
    };
}