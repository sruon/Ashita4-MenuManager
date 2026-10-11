#pragma once

#include <cstdint>

namespace menumanager::FFXI
{

enum class MenuFade : uint8_t
{
    Shown  = 0,
    Hidden = 1,
    Out    = 2,
    In     = 3,
};

} // namespace menumanager::FFXI
