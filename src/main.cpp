#include <iostream>
#include <cstring>
#include <cassert>

#include "common.h"
#include "page.h"
#include "disk_manager.h"
#include "buffer_pool.h"

void test_common();
void test_page();
void test_disk_manager();
void test_buffer_pool();

int main()
{
    std::cout << "==================================" << std::endl;
    std::cout << "  Mini-DB Storage Engine Tests   " << std::endl;
    std::cout << "==================================" << std::endl;

    try
    {
        test_common();
        test_page();
        test_disk_manager();
        test_buffer_pool();

        std::cout << "\n==================================" << std::endl;
        std::cout << "  ✓ All Tests Passed!            " << std::endl;
        std::cout << "==================================" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n✗ Test Failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void test_common()
{
    std::cout << "\n[1/4] Testing common.h" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    std::cout << "  PAGE_SIZE: " << minidb::PAGE_SIZE << " bytes" << std::endl;
    std::cout << "  INVALID_PAGE_ID: " << minidb::INVALID_PAGE_ID << std::endl;

    minidb::page_id_t pid = 5;
    assert(pid == 5);
    std::cout << "  ✓ page_id_t type works" << std::endl;

    minidb::frame_id_t fid = 10;
    assert(fid == 10);
    std::cout << "  ✓ frame_id_t type works" << std::endl;
}

void test_page()
{
    std::cout << "\n[2/4] Testing Page" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    // Test default initialization
    minidb::Page page;
    assert(page.GetPageId() == minidb::INVALID_PAGE_ID);
    assert(page.GetPinCount() == 0);
    assert(page.IsDirty() == false);
    std::cout << "  ✓ Default initialization" << std::endl;

    // Test data read/write
    const char *test_str = "Hello, Mini-DB!";
    strcpy(page.GetData(), test_str);
    assert(strcmp(page.GetData(), test_str) == 0);
    std::cout << "  ✓ Data read/write" << std::endl;

    // Test setters
    page.SetPageId(42);
    assert(page.GetPageId() == 42);

    page.IncrementPinCount();
    page.IncrementPinCount();
    assert(page.GetPinCount() == 2);

    page.DecrementPinCount();
    assert(page.GetPinCount() == 1);

    page.SetDirty(true);
    assert(page.IsDirty() == true);
    std::cout << "  ✓ Setters and getters" << std::endl;

    // Test reset
    page.Reset();
    assert(page.GetPageId() == minidb::INVALID_PAGE_ID);
    assert(page.GetPinCount() == 0);
    assert(page.IsDirty() == false);
    assert(page.GetData()[0] == '\0');
    std::cout << "  ✓ Reset functionality" << std::endl;
}

void test_disk_manager()
{
    std::cout << "\n[3/4] Testing DiskManager" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    minidb::DiskManager dm("data/test.db");

    // Test page allocation
    minidb::page_id_t p0 = dm.AllocatePage();
    minidb::page_id_t p1 = dm.AllocatePage();
    minidb::page_id_t p2 = dm.AllocatePage();
    assert(p0 == 0 && p1 == 1 && p2 == 2);
    std::cout << "  ✓ Page allocation (allocated: " << p0 << ", " << p1 << ", " << p2 << ")" << std::endl;

    // Test write and read
    char write_buf[minidb::PAGE_SIZE] = {0};
    strcpy(write_buf, "Persistent data on disk!");
    dm.WritePage(p0, write_buf);

    char read_buf[minidb::PAGE_SIZE] = {0};
    dm.ReadPage(p0, read_buf);
    assert(strcmp(write_buf, read_buf) == 0);
    std::cout << "  ✓ Write and read (verified: \"" << read_buf << "\")" << std::endl;

    // Test multiple pages
    strcpy(write_buf, "Page 1 content");
    dm.WritePage(p1, write_buf);
    strcpy(write_buf, "Page 2 content");
    dm.WritePage(p2, write_buf);

    dm.ReadPage(p1, read_buf);
    assert(strcmp(read_buf, "Page 1 content") == 0);
    dm.ReadPage(p2, read_buf);
    assert(strcmp(read_buf, "Page 2 content") == 0);
    std::cout << "  ✓ Multiple page persistence" << std::endl;
}

void test_buffer_pool()
{
    std::cout << "\n[4/4] Testing BufferPool" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    minidb::DiskManager dm("data/test_bp.db");
    minidb::buffer_pool pool(3, &dm);

    // Test 1: NewPage
    std::cout << "  [4.1] New page creation..." << std::endl;
    minidb::page_id_t pid1, pid2, pid3;
    minidb::Page *p1 = pool.NewPage(&pid1);
    minidb::Page *p2 = pool.NewPage(&pid2);
    minidb::Page *p3 = pool.NewPage(&pid3);

    strcpy(p1->GetData(), "Page 1");
    strcpy(p2->GetData(), "Page 2");
    strcpy(p3->GetData(), "Page 3");

    assert(p1->GetPinCount() == 1);
    assert(p1->GetPageId() == pid1);
    std::cout << "    ✓ Created pages: " << pid1 << ", " << pid2 << ", " << pid3 << std::endl;

    pool.UnpinPage(pid1, true);
    pool.UnpinPage(pid2, true);
    pool.UnpinPage(pid3, true);
    assert(p1->GetPinCount() == 0);
    std::cout << "    ✓ Unpinned all pages" << std::endl;

    // Test 2: FetchPage (cache hit)
    std::cout << "  [4.2] Cache hit test..." << std::endl;
    minidb::Page *fetch = pool.FetchPage(pid1);
    assert(fetch != nullptr);
    assert(strcmp(fetch->GetData(), "Page 1") == 0);
    assert(fetch->GetPinCount() == 1);
    std::cout << "    ✓ Fetched cached page: \"" << fetch->GetData() << "\"" << std::endl;
    pool.UnpinPage(pid1, false);

    // Test 3: Page eviction
    std::cout << "  [4.3] Eviction test (pool size=3)..." << std::endl;
    minidb::page_id_t pid4;
    minidb::Page *p4 = pool.NewPage(&pid4);
    strcpy(p4->GetData(), "Page 4 evicts LRU");
    std::cout << "    ✓ Created 4th page (id=" << pid4 << "), should evict LRU" << std::endl;
    pool.UnpinPage(pid4, true);

    // Test 4: Fetch evicted page from disk
    std::cout << "  [4.4] Fetch evicted page..." << std::endl;
    minidb::Page *refetch = pool.FetchPage(pid2);
    assert(refetch != nullptr);
    assert(strcmp(refetch->GetData(), "Page 2") == 0);
    std::cout << "    ✓ Re-fetched evicted page: \"" << refetch->GetData() << "\"" << std::endl;
    pool.UnpinPage(pid2, false);

    // Test 5: Dirty page flush
    std::cout << "  [4.5] Flush dirty page..." << std::endl;
    minidb::Page *modify = pool.FetchPage(pid3);
    strcpy(modify->GetData(), "Modified Page 3");
    pool.UnpinPage(pid3, true);
    pool.FlushPage(pid3);
    std::cout << "    ✓ Flushed modified page to disk" << std::endl;

    // Test 6: Multiple pins
    std::cout << "  [4.6] Multiple pin count..." << std::endl;
    minidb::Page *pin1 = pool.FetchPage(pid4);
    minidb::Page *pin2 = pool.FetchPage(pid4);
    assert(pin1 == pin2);
    assert(pin1->GetPinCount() == 2);
    std::cout << "    ✓ Same page pinned twice (pin_count=" << pin1->GetPinCount() << ")" << std::endl;
    pool.UnpinPage(pid4, false);
    pool.UnpinPage(pid4, false);

    // Test 7: Flush all
    std::cout << "  [4.7] Flush all pages..." << std::endl;
    pool.FlushAllPages();
    std::cout << "    ✓ Flushed all dirty pages" << std::endl;

    // Test 8: Stress test
    std::cout << "  [4.8] Stress test (10 pages)..." << std::endl;
    int created_count = 0;
    for (int i = 0; i < 10; i++)
    {
        minidb::page_id_t stress_pid;
        minidb::Page *sp = pool.NewPage(&stress_pid);
        if (sp != nullptr)
        {
            sprintf(sp->GetData(), "Stress page %d", i);
            pool.UnpinPage(stress_pid, true);
            created_count++;
        }
    }
    std::cout << "    ✓ Created " << created_count << " pages with evictions" << std::endl;

    // Test 9: Random access pattern
    std::cout << "  [4.9] Random access pattern..." << std::endl;
    minidb::page_id_t access_order[] = {pid1, pid4, pid2, pid3, pid1};
    int access_count = 0;
    for (int i = 0; i < 5; i++)
    {
        minidb::Page *ap = pool.FetchPage(access_order[i]);
        if (ap != nullptr)
        {
            pool.UnpinPage(access_order[i], false);
            access_count++;
        }
    }
    std::cout << "    ✓ Accessed " << access_count << " pages in random order" << std::endl;
}