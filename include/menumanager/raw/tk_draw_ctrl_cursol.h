#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A window's cursor
struct CTkDrawCtrlCursol
{
    int16_t m_RecKind;
    uint8_t pad02[0x1E];
    int16_t m_LocX, m_LocY;
};

static_assert(offsetof(CTkDrawCtrlCursol, m_LocX) == 0x20);

#pragma pack(pop)

} // namespace menumanager::raw
