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
namespace minidb
{
    class DiskManager
    {
    public:
        DiskManager(const std::string &db_file);
        ~DiskManager();

        void ReadPage(page_id_t page_id, char *page_data);
        void WritePage(page_id_t page_id, const char *page_data);
        page_id_t AllocatePage();

    private:
        std::fstream db_file_;
        page_id_t next_page_id_;
    };
}