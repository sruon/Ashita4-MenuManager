#pragma once

#include <cstdint>

namespace menumanager::FFXI
{

// The scroll state of a list window (inventory, storage, most item pickers).
// Rows Top to Top + PageSize - 1 are on screen, and the selected item is Top + Cursor - 1
struct menulist_t
{
    uintptr_t Address;
    int16_t   Columns;
    int16_t   PageSize; // rows per page
    int16_t   Start;    // first and last row drawn
    int16_t   End;
    int16_t   Top;   // first row shown: the scroll position
    int16_t   Count; // number of items
    int16_t   Scroll;
    int16_t   LastCursor;
    int16_t   RowHeight; // in menu pixels
    int16_t   X;         // where the rows start
    int16_t   Y;
};

} // namespace menumanager::FFXI
