#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A window's button
struct CTkDrawCtrlButton
{
    int16_t  m_RecKind;
    uint8_t  pad02[2];
    uint32_t m_pShape;
    uint32_t m_pUserShape; // replacement art, set by window style settings
    uint32_t m_pMB;        // CTkMenuButton
    uint32_t m_MenuCtrlID;
    uint8_t  pad14[0x16];
    int16_t  m_LocX, m_LocY, m_Width, m_Height; // on-screen position and size
    uint8_t  pad32[8];
    int16_t  m_ButtonNo; // +0x32 on PS2
};

static_assert(offsetof(CTkDrawCtrlButton, m_pShape) == 0x04 && offsetof(CTkDrawCtrlButton, m_pUserShape) == 0x08 && offsetof(CTkDrawCtrlButton, m_pMB) == 0x0C);
static_assert(offsetof(CTkDrawCtrlButton, m_MenuCtrlID) == 0x10 && offsetof(CTkDrawCtrlButton, m_LocX) == 0x2A && offsetof(CTkDrawCtrlButton, m_ButtonNo) == 0x3A);

#pragma pack(pop)

} // namespace menumanager::raw
