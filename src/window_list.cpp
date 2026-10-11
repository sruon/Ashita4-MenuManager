#include "menu_manager.h"

#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_menu_ctrl_data.h>
#include <menumanager/raw/yk_wnd_list.h>

#include <cstddef>

namespace menumanager
{

using namespace raw;

auto MenuManager::GetWindowList(const uintptr_t window, FFXI::menulist_t* list) const -> bool
{
    uint32_t  instance = 0;
    YkWndList wnd{};
    if (list == nullptr || window < kPointerFloor || !read(window + offsetof(CTkMenuCtrlData, m_pMP), instance) || !read(instance, wnd))
    {
        return false;
    }

    *list = { .Address    = instance,
              .Columns    = wnd.m_column,
              .PageSize   = wnd.m_pagemax,
              .Start      = wnd.m_start,
              .End        = wnd.m_end,
              .Top        = wnd.m_listcur,
              .Count      = wnd.m_listmax,
              .Scroll     = wnd.m_listscr,
              .LastCursor = wnd.m_lastcur,
              .RowHeight  = wnd.m_h,
              .X          = wnd.m_x,
              .Y          = wnd.m_y };
    // the window class isn't known, so judge by whether the values make sense for a list
    return list->PageSize > 0 && list->Top >= 0 && list->Count >= list->Top && list->End <= list->Count;
}

} // namespace menumanager
