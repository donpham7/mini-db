#include <cstdint>       // int32_t, uint64_t
#include <cstring>       // memset, memcpy, strcmp
#include <string>        // std::string
#include <vector>        // std::vector
#include <list>          // std::list
#include <unordered_map> // std::unordered_map
#include <fstream>       // std::fstream
#include <stdexcept>     // std::runtime_error
#include <iostream>      // std::cout

#pragma once

namespace minidb
{
    /// @brief Page identifier on disk memory
    using page_id_t = int32_t;

    /// @brief Slot identifier in buffer pool
    using frame_id_t = int32_t;

    /// @brief Bytes per page
    const int32_t PAGE_SIZE = 4096;

    /// @brief ERROR: No page found
    const int32_t INVALID_PAGE_ID = -1;

    /// @brief ERROR: No frame found
    const int32_t INVALID_FRAME_ID = -1;
}