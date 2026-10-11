#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A window's frame. Every draw object starts with m_RecKind: 0 frame, 1 button, 2 cursor
struct CTkDrawCtrlFrame
{
    int16_t  m_RecKind;
    uint8_t  pad02[6];
    uint32_t m_pShape; // the art being drawn
    uint8_t  pad0C[0x1A];
    int16_t  m_NeckTime; // frames left of the opening animation (starts at 25)
    uint8_t  pad28[8];
    uint32_t m_pMF; // CTkMenuFrame
    uint8_t  pad34[0x10];
    uint8_t  m_fadeMode; // 0 shown, 1 hidden, 2 fading out, 3 fading in
    uint8_t  pad45[3];
    int32_t  m_fadeTime; // frames left of the fade
};

static_assert(offsetof(CTkDrawCtrlFrame, m_pShape) == 0x08 && offsetof(CTkDrawCtrlFrame, m_NeckTime) == 0x26 && offsetof(CTkDrawCtrlFrame, m_pMF) == 0x30);
static_assert(offsetof(CTkDrawCtrlFrame, m_fadeMode) == 0x44 && offsetof(CTkDrawCtrlFrame, m_fadeTime) == 0x48);

#pragma pack(pop)

} // namespace menumanager::raw
