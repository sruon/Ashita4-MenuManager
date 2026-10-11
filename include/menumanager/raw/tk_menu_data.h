#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A menu's definition, loaded from the DAT
struct CTkMenuData
{
    uint8_t pad00[0x46];
    char    m_ID[16]; // "menu    inventor"
};

static_assert(offsetof(CTkMenuData, m_ID) == 0x46);

#pragma pack(pop)

} // namespace menumanager::raw
