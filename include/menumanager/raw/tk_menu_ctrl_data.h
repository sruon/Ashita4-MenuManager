#pragma once

#include <menumanager/raw/tk_ob_list.h>

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// An open menu window
struct CTkMenuCtrlData
{
    uint8_t   pad00[4];
    uint32_t  m_pMD; // CTkMenuData
    uint8_t   pad08[4];
    uint32_t  m_pMP;        // the window's class instance (CTkMenuPrimitive)
    uint32_t  m_MenuCtrlID; // 0xE while closing
    CTkObList m_DrawShapes; // what the window draws
    uint8_t   pad30[0x0A];
    int16_t   m_WinRect[4];
    uint8_t   pad42[0x0A];
    int16_t   m_CursolNum;
    uint8_t   pad4E[0x0A];
    int16_t   m_ButtonSuu;
    uint8_t   pad5A[6];
    uint8_t   m_Priority;
    uint8_t   pad61;
    uint8_t   m_EnableCursolDraw; // cleared while a text field has focus
};

static_assert(offsetof(CTkMenuCtrlData, m_pMD) == 0x04 && offsetof(CTkMenuCtrlData, m_pMP) == 0x0C && offsetof(CTkMenuCtrlData, m_MenuCtrlID) == 0x10);
static_assert(offsetof(CTkMenuCtrlData, m_DrawShapes) == 0x14 && offsetof(CTkMenuCtrlData, m_WinRect) == 0x3A && offsetof(CTkMenuCtrlData, m_CursolNum) == 0x4C);
static_assert(offsetof(CTkMenuCtrlData, m_ButtonSuu) == 0x58 && offsetof(CTkMenuCtrlData, m_Priority) == 0x60 && offsetof(CTkMenuCtrlData, m_EnableCursolDraw) == 0x62);

#pragma pack(pop)

} // namespace menumanager::raw
