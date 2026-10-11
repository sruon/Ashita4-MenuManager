#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// The text of the help line and the title box. The PS2 layout has m_pDisp at +0x10 and no anims.
// The highest level with text is shown, and a message wider than the line gets two spaces appended so it can scroll
struct KaHelpMes
{
    uint8_t  m_anm[0x48]; // drawn centred on the line instead of the text while a load bit is set (KaHelpMes::Draw)
    uint32_t m_pDisp;
    uint8_t  pad4C[4];
    int32_t  m_mesW;
    uint8_t  pad54[0x10];
    uint32_t m_loadBit; // KaHelpMes::SetLoadMes ORs bits in. 0x80000000 is the auction indicator
    uint32_t anims[2];  // two more animations, drawn over the text
    int32_t  m_dispLevel;
    char     levels[4][0xCB];
    uint8_t  pad3A0[4];
};

static_assert(offsetof(KaHelpMes, m_pDisp) == 0x48 && offsetof(KaHelpMes, m_mesW) == 0x50 && offsetof(KaHelpMes, m_loadBit) == 0x64);
static_assert(offsetof(KaHelpMes, m_dispLevel) == 0x70 && offsetof(KaHelpMes, levels) == 0x74 && sizeof(KaHelpMes) == 0x3A4);

#pragma pack(pop)

} // namespace menumanager::raw
