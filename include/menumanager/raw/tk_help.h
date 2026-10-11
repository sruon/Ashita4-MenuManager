#pragma once

#include <menumanager/raw/ka_help_mes.h>

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// The help line's window class (helpwind)
struct CTkHelp
{
    uint32_t  vtable;
    uint8_t   pad04[0x1C];
    KaHelpMes m_helpMes;
};

static_assert(offsetof(CTkHelp, m_helpMes) == 0x20);

#pragma pack(pop)

} // namespace menumanager::raw
