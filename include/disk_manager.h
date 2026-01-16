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
    class DiskManager
    {
    public:
        /// @brief Opens DB file and manages page ID with file sizes
        /// @param db_file DB file to open
        DiskManager(const std::string &db_file);

        /// @brief Closes DB file
        ~DiskManager();

        /// @brief Reads page from disk into buffer pool. Called on cache miss
        /// @param page_id Page ID to insert into buffer pool
        /// @param page_data Page data to insert into frame/buffer pool
        void ReadPage(page_id_t page_id, char *page_data);

        /// @brief Writes to disk from buffer. Buffer pool calls this when flushing dirty pages
        /// @param page_id Which page ID to write to
        /// @param page_data Page to data to flush
        void WritePage(page_id_t page_id, const char *page_data);

        /// @brief Returns next Page ID and extends file. Buffer pool calls this when creating new pages
        /// @return Next page ID
        page_id_t AllocatePage();

        /// @brief Returns next page ID
        /// @return next_page_id_
        inline page_id_t GetNumPages()
        {
            return next_page_id_;
        }

    private:
        /// @brief DB file stream
        std::fstream db_file_;
        /// @brief Next page ID
        page_id_t next_page_id_;
    };
}