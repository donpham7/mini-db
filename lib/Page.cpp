#include "common.h"
#include "../include/page.h"

namespace minidb
{

    void Page::Reset()
    {
        memset(data_, 0, PAGE_SIZE);
        page_id_ = INVALID_PAGE_ID;
        pin_count_ = 0;
        is_dirty_ = false;
    }

}