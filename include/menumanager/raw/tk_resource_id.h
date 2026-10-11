#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// One piece of art a frame or button can draw: an art file's id and an index into it.
// 28 bytes in memory, 20 in the DAT. m_Shapes points at the loaded file, or is 0 if it isn't loaded
struct CTkResourceID
{
    int16_t  m_kind, m_RscNum;
    char     m_RscID[16];
    uint8_t  pad14[4];
    uint32_t m_Shapes; // YmMenuShapeFile
};

static_assert(sizeof(CTkResourceID) == 28);

#pragma pack(pop)

} // namespace menumanager::raw
