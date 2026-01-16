// src/main.cpp
#include <iostream>
#include <cstring>

#include "common.h"

// Uncomment as you implement:
#include "page.h"
// #include "disk_manager.h"
// #include "buffer_pool.h"
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
    // std::cout << "\n--- Testing DiskManager ---" << std::endl;
    // DiskManager dm("data/test.db");
    //
    // page_id_t p0 = dm.AllocatePage();
    // page_id_t p1 = dm.AllocatePage();
    // std::cout << "Allocated: " << p0 << ", " << p1 << std::endl;
    //
    // char write_buf[PAGE_SIZE] = {0};
    // strcpy(write_buf, "Hello from disk!");
    // dm.WritePage(p0, write_buf);
    // std::cout << "Wrote to page " << p0 << std::endl;
    //
    // char read_buf[PAGE_SIZE] = {0};
    // dm.ReadPage(p0, read_buf);
    // std::cout << "Read back: " << read_buf << std::endl;

    // === Test 4: buffer_pool.h ===
    // std::cout << "\n--- Testing BufferPool ---" << std::endl;
    // DiskManager dm2("data/test_bp.db");
    // BufferPool pool(3, &dm2);
    //
    // page_id_t new_pid;
    // Page* pg = pool.NewPage(&new_pid);
    // std::cout << "NewPage returned page: " << new_pid << std::endl;
    //
    // strcpy(pg->GetData(), "Buffered!");
    // pool.UnpinPage(new_pid, true);
    // pool.FlushPage(new_pid);
    //
    // Page* fetched = pool.FetchPage(new_pid);
    // std::cout << "Fetched data: " << fetched->GetData() << std::endl;
    // pool.UnpinPage(new_pid, false);

    std::cout << "\nDone!" << std::endl;
    return 0;
}