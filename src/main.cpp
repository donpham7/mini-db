// src/main.cpp
#include <iostream>
#include <cstring>

#include "common.h"

// Uncomment as you implement:
#include "page.h"
#include "disk_manager.h"
#include "buffer_pool.h"
int main()
{
    std::cout << "Mini-DB Storage Engine" << std::endl;
    std::cout << "======================" << std::endl;

    // === Test 1: common.h ===
    std::cout << "\n--- Testing common.h ---" << std::endl;
    std::cout << "PAGE_SIZE: " << minidb::PAGE_SIZE << std::endl;
    std::cout << "INVALID_PAGE_ID: " << minidb::INVALID_PAGE_ID << std::endl;

    minidb::page_id_t pid = 5;
    std::cout << "page_id_t works: " << pid << std::endl;

    // === Test 2: page.h ===
    std::cout << "\n--- Testing Page ---" << std::endl;
    minidb::Page page;
    std::cout << "Initial page_id: " << page.GetPageId() << std::endl;
    std::cout << "Initial pin_count: " << page.GetPinCount() << std::endl;
    std::cout << "Initial is_dirty: " << page.IsDirty() << std::endl;

    strcpy(page.GetData(), "Hello, Page!");
    std::cout << "Data written: " << page.GetData() << std::endl;

    page.SetPageId(42);
    page.IncrementPinCount();
    page.SetDirty(true);
    std::cout << "After modifications - id: " << page.GetPageId()
              << ", pins: " << page.GetPinCount()
              << ", dirty: " << page.IsDirty() << std::endl;

    page.Reset();
    std::cout << "After Reset - id: " << page.GetPageId() << std::endl;

    // === Test 3: disk_manager.h ===
    std::cout << "\n--- Testing DiskManager ---" << std::endl;
    minidb::DiskManager dm("data/test.db");

    minidb::page_id_t p0 = dm.AllocatePage();
    minidb::page_id_t p1 = dm.AllocatePage();
    std::cout << "Allocated: " << p0 << ", " << p1 << std::endl;

    char write_buf[minidb::PAGE_SIZE] = {0};
    strcpy(write_buf, "Hello from disk!");
    std::cout << "Attempting to write " << write_buf << " to page " << p0 << std::endl;
    dm.WritePage(p0, write_buf);
    std::cout << "Wrote to page " << p0 << std::endl;

    char read_buf[minidb::PAGE_SIZE] = {0};
    dm.ReadPage(p0, read_buf);
    std::cout << "Read back: " << read_buf << std::endl;

    // === Test 4: buffer_pool.h ===
    std::cout << "\n--- Testing BufferPool ---" << std::endl;
    minidb::DiskManager dm2("data/test_bp.db");
    minidb::buffer_pool pool(3, &dm2);

    // Test 4.1: Create multiple pages
    std::cout << "\n[Test 4.1] Creating multiple pages" << std::endl;
    minidb::page_id_t pid1, pid2, pid3;
    minidb::Page *page1 = pool.NewPage(&pid1);
    minidb::Page *page2 = pool.NewPage(&pid2);
    minidb::Page *page3 = pool.NewPage(&pid3);
    strcpy(page1->GetData(), "Page 1");
    strcpy(page2->GetData(), "Page 2");
    strcpy(page3->GetData(), "Page 3");
    std::cout << "Created pages: " << pid1 << ", " << pid2 << ", " << pid3 << std::endl;
    pool.UnpinPage(pid1, true);
    pool.UnpinPage(pid2, true);
    pool.UnpinPage(pid3, true);

    // Test 4.2: Fetch existing pages (cache hit)
    std::cout << "\n[Test 4.2] Cache hit test" << std::endl;
    minidb::Page *fetch1 = pool.FetchPage(pid1);
    std::cout << "Fetched page " << pid1 << ": " << fetch1->GetData() << std::endl;
    pool.UnpinPage(pid1, false);

    // Test 4.3: Force eviction (pool size is 3, allocate 4th)
    std::cout << "\n[Test 4.3] Testing eviction (pool size=3, allocating 4th)" << std::endl;
    minidb::page_id_t pid4;
    minidb::Page *p4 = pool.NewPage(&pid4);
    strcpy(p4->GetData(), "Page 4 - should evict LRU");
    std::cout << "Created page " << pid4 << std::endl;
    pool.UnpinPage(pid4, true);

    // Test 4.4: Verify evicted page can be fetched from disk
    std::cout << "\n[Test 4.4] Fetching evicted page from disk" << std::endl;
    minidb::Page *refetch2 = pool.FetchPage(pid2);
    std::cout << "Re-fetched page " << pid2 << ": " << refetch2->GetData() << std::endl;
    pool.UnpinPage(pid2, false);

    // Test 4.5: Modify and flush
    std::cout << "\n[Test 4.5] Modify and flush" << std::endl;
    minidb::Page *mod = pool.FetchPage(pid3);
    strcpy(mod->GetData(), "Modified Page 3");
    pool.UnpinPage(pid3, true);
    pool.FlushPage(pid3);
    std::cout << "Modified and flushed page " << pid3 << std::endl;

    // Test 4.6: Multiple pins on same page
    std::cout << "\n[Test 4.6] Multiple pins on same page" << std::endl;
    minidb::Page *pin1 = pool.FetchPage(pid4);
    minidb::Page *pin2 = pool.FetchPage(pid4);
    std::cout << "Page " << pid4 << " pin count: " << pin1->GetPinCount() << std::endl;
    pool.UnpinPage(pid4, false);
    pool.UnpinPage(pid4, false);

    // Test 4.7: FlushAll
    std::cout << "\n[Test 4.7] Flush all pages" << std::endl;
    pool.FlushAllPages();
    std::cout << "Flushed all dirty pages" << std::endl;

    // Test 4.8: Stress test - create many pages
    std::cout << "\n[Test 4.8] Stress test - allocate 10 pages" << std::endl;
    for (int i = 0; i < 10; i++)
    {
        minidb::page_id_t stress_pid;
        minidb::Page *sp = pool.NewPage(&stress_pid);
        sprintf(sp->GetData(), "Stress page %d", i);
        pool.UnpinPage(stress_pid, true);
        if (i % 3 == 0)
        {
            std::cout << "Created page " << stress_pid << std::endl;
        }
    }

    // Test 4.9: Random access pattern
    std::cout << "\n[Test 4.9] Random access pattern" << std::endl;
    minidb::page_id_t access_order[] = {pid1, pid4, pid2, pid3, pid1};
    for (int i = 0; i < 5; i++)
    {
        minidb::Page *ap = pool.FetchPage(access_order[i]);
        std::cout << "Accessed page " << access_order[i] << std::endl;
        pool.UnpinPage(access_order[i], false);
    }

    std::cout << "\n--- BufferPool Tests Complete ---" << std::endl;

    std::cout << "\nDone!" << std::endl;
    return 0;
}