#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// The window class behind list windows (inventory, storage, item pickers). Same layout as on PS2
struct YkWndList
{
    uint8_t pad00[0x16];
    int16_t m_column, m_pagemax, m_start, m_end, m_listcur, m_listmax, m_listscr, m_lastcur, m_h, m_x, m_y;
};

static_assert(offsetof(YkWndList, m_column) == 0x16 && offsetof(YkWndList, m_y) == 0x2A);

#pragma pack(pop)

} // namespace menumanager::raw
