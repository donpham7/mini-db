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

#pragma once

namespace minidb
{
    class Page
    {
    public:
        /// @brief Gets data from page
        /// @return data from page
        inline char *GetData()
        {
            return data_;
        }

        /// @brief Gets Page ID
        /// @return Page ID
        inline page_id_t GetPageId()
        {
            return page_id_;
        }

        /// @brief Sets Page ID
        /// @param new_page_id Page ID to set to
        inline void SetPageId(page_id_t new_page_id)
        {
            page_id_ = new_page_id;
        }

        /// @brief Gets pin count
        /// @return Pin count - Number of operations using page
        inline int GetPinCount()
        {
            return pin_count_;
        }

        /// @brief Increases pin count by 1
        inline void IncrementPinCount()
        {
            pin_count_++;
        }

        /// @brief Decreases pin count by 1
        inline void DecrementPinCount()
        {
            pin_count_--;
        }

        /// @brief Gets dirty status
        /// @return Is dirty - Has been modified
        inline bool IsDirty()
        {
            return is_dirty_;
        }

        /// @brief Sets dirty status
        /// @param isDirty Has been modified
        inline void SetDirty(bool is_dirty)
        {
            is_dirty_ = is_dirty;
        }

        /// @brief Resets page for when frame is used for another page
        void Reset();

    private:
        /// @brief Data stored within page. Defaulted to zeros
        char data_[PAGE_SIZE] = {};

        /// @brief Page ID. Defaulted to -1, Invalid Page ID
        page_id_t page_id_ = INVALID_PAGE_ID;

        /// @brief Tracks how many operations are using this page. Cannot evict if > 0
        int pin_count_ = 0;

        /// @brief Tracks if page was modified. Write to disk if modified.
        bool is_dirty_ = false;
    };
}