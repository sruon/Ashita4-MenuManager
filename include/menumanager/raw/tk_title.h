#pragma once

#include <menumanager/raw/ka_help_mes.h>

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// The title box's window class (titlewin)
struct CTkTitle
{
    uint32_t  vtable;
    uint8_t   pad04[0xA0];
    KaHelpMes m_helpMes;
    int16_t   m_drawmode; // the text shows only in mode 0. Mode 1 shows the POL handle
};

static_assert(offsetof(CTkTitle, m_helpMes) == 0xA4 && offsetof(CTkTitle, m_drawmode) == 0x448);

#pragma pack(pop)

} // namespace menumanager::raw
