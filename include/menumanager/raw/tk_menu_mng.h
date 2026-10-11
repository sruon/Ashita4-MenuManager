#pragma once

#include <menumanager/raw/tk_ob_list.h>

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// The menu manager: every open window, and which one has focus
struct CTkMenuMng
{
    CTkObList m_DrawDatas; // open windows
    uint8_t   pad1C[0x38];
    uint32_t  m_pCurMCD;
    uint8_t   pad58[0x24];
    int16_t   m_WinRect[4]; // the menu area (+0x9C on PS2)
};

static_assert(offsetof(CTkMenuMng, m_pCurMCD) == 0x54 && offsetof(CTkMenuMng, m_WinRect) == 0x7C);

#pragma pack(pop)

} // namespace menumanager::raw
