#pragma once

#include <cstdint>

namespace menumanager::FFXI
{

// Keys for IMenuManager::PressKey
enum class MenuKey : int32_t
{
    Right  = 3,
    Left   = 4,
    Ok     = 5,
    Cancel = 6,
};

} // namespace menumanager::FFXI
