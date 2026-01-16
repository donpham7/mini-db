#include <cstdint>       // int32_t, uint64_t
#include <cstring>       // memset, memcpy, strcmp
#include <string>        // std::string
#include <vector>        // std::vector
#include <list>          // std::list
#include <unordered_map> // std::unordered_map
#include <fstream>       // std::fstream
#include <stdexcept>     // std::runtime_error
#include <iostream>      // std::cout
#include "../include/DiskManager.h"

namespace minidb
{
    void DiskManager::ReadPage(page_id_t page_id, char *page_data)
    {
        db_file_.seekg(page_id * PAGE_SIZE);
        db_file_.read(page_data, PAGE_SIZE);
    }
}