#include "../include/buffer_pool.h"

namespace minidb
{
    buffer_pool::buffer_pool(int frames, DiskManager *dm)
        : pool_size_(frames), disk_manager_(dm), pages_(frames)
    {
        for (int i = 0; i < frames; i++)
        {
            free_list.push_front(i);
        }
    }

    Page *buffer_pool::FetchPage(page_id_t page_id)
    {
        // If cache hit
        if (page_table_.find(page_id) != page_table_.end())
        {
            // Get page and pin for use
            Page *page = &pages_[page_table_[page_id]];
            page->IncrementPinCount();

            // Update LRU
            buffer_pool::UpdateLRU(page_table_[page_id]);
            return page;
        }

        // Get available frame_id for page from disk
        // If cache miss
        // Get a frame if any are available in free list, else evict
        frame_id_t frame_id = 0;
        if (!free_list.empty())
        {
            // Find free frame
            frame_id = free_list.front();
            free_list.pop_front();
        }
        else // No free frame, evict page to free a frame
        {
            bool foundVictim = FindVictim(&frame_id);
            if (foundVictim)
            {
                Page victim = pages_[frame_id];
                if (victim.IsDirty())
                {
                    FlushPage(victim.GetPageId());
                    page_table_.erase(victim.GetPageId());
                    pages_[frame_id].Reset();
                    RemoveFromLRU(frame_id);
                }
            }
            else
            {
                throw std::runtime_error("Failed to fetch page, No free and no victim");
            }
        }
        // Read page into memory
        char page_data[PAGE_SIZE] = {0};
        disk_manager_->ReadPage(page_id, page_data);

        // Overwrite free page in cache
        // Update Page settings
        memcpy(pages_[frame_id].GetData(), page_data, PAGE_SIZE);
        pages_[frame_id].SetPageId(page_id);
        pages_[frame_id].IncrementPinCount();

        page_table_[page_id] = frame_id;
        AddToLRU(frame_id);
        return &pages_[frame_id];
    }

    Page *buffer_pool::NewPage(page_id_t *page_id)
    {
        // Get a frame
        frame_id_t frame_id = 0;
        if (!free_list.empty())
        {
            // Find free frame
            frame_id = free_list.front();
            free_list.pop_front();
        }
        else // No free frame, evict page to free a frame
        {
            bool foundVictim = FindVictim(&frame_id);
            if (foundVictim)
            {
                Page victim = pages_[frame_id];
                if (victim.IsDirty())
                {
                    FlushPage(victim.GetPageId());
                }
                page_table_.erase(victim.GetPageId());
                pages_[frame_id].Reset();
                RemoveFromLRU(frame_id);
            }
            else
            {
                throw std::runtime_error("Failed to create new page, No free and no victim");
            }
        }
        // New page
        *page_id = disk_manager_->AllocatePage();

        // erase page from cache
        pages_[frame_id].Reset();

        // Setup new page
        pages_[frame_id].SetPageId(*page_id); // ID

        // Update page table and LRU
        page_table_[*page_id] = frame_id;
        AddToLRU(frame_id);
        pages_[frame_id].IncrementPinCount();
        return &pages_[frame_id];
    }

    void buffer_pool::UnpinPage(page_id_t page_id, bool isDirty)
    {
        pages_[page_table_[page_id]].DecrementPinCount();
        pages_[page_table_[page_id]].SetDirty(isDirty);
    }

    void buffer_pool::FlushPage(page_id_t page_id)
    {
        disk_manager_->WritePage(page_id, pages_[page_table_[page_id]].GetData());
        pages_[page_table_[page_id]].SetDirty(false);
    }

    void buffer_pool::FlushAllPages()
    {
        for (size_t i = 0; i < pool_size_; i++)
        {
            Page page = pages_.at(i);
            if (page.GetPageId() != INVALID_PAGE_ID)
            {
                page_id_t page_id = page.GetPageId();
                disk_manager_->WritePage(page_id, pages_[page_table_[page_id]].GetData());
                pages_[page_table_[page_id]].SetDirty(false);
            }
        }
    }

} // namespace minidb
