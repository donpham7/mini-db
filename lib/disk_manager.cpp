#include "../include/disk_manager.h"

namespace minidb
{
    DiskManager::DiskManager(const std::string &db_file)
    {
        db_file_.open(db_file, std::ios::in | std::ios::out | std::ios::binary);

        // If file is not open, then file does not exist
        if (!db_file_.is_open())
        {
            // Creates file
            db_file_.open(db_file, std::ios::out | std::ios::binary);
            db_file_.close();

            // Retry open
            db_file_.open(db_file, std::ios::in | std::ios::out | std::ios::binary);
        }

        if (!db_file_.is_open())
        {
            throw std::runtime_error("Failed to open file " + db_file);
        }

        next_page_id_ = 0;
    }

    DiskManager::~DiskManager()
    {
        db_file_.close();
    }

    void DiskManager::ReadPage(page_id_t page_id, char *page_data)
    {
        // Validate that page_id request is valid
        if (page_id < next_page_id_ && page_id >= 0)
        {
            // Seek to read position
            int read_position = page_id * PAGE_SIZE;
            db_file_.seekg(read_position, std::ios::beg);

            // Read file
            db_file_.read(page_data, PAGE_SIZE);
            std::cout << page_data << std::endl;
        }
    }

    void DiskManager::WritePage(page_id_t page_id, const char *page_data)
    {
        // Validate write position
        if (page_id < next_page_id_ && page_id >= 0)
        {
            // Seek
            int write_position = page_id * PAGE_SIZE;
            db_file_.seekp(write_position, std::ios::beg);

            // Write to the file
            db_file_.write(page_data, PAGE_SIZE);

            db_file_.flush();
        }
    }

    page_id_t DiskManager::AllocatePage()
    {
        // Return the created page ID and increment next ID
        int created_page_id = next_page_id_;
        next_page_id_++;

        // Create empty buffer and extend file
        char emptyBuffer[PAGE_SIZE];
        memset(emptyBuffer, 0, PAGE_SIZE);
        db_file_.seekp(std::ios::end);
        db_file_.write(emptyBuffer, PAGE_SIZE);

        return created_page_id;
    }

}