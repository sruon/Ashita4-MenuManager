#include "menu_manager.h"

#include <menumanager/raw/memory.h>
#include <menumanager/window_state.h>

#include <cstdint>

namespace menumanager
{

using namespace raw;

namespace
{

// Open, and not in its opening or closing animation
auto settled(const IMenuManager& menus, const uintptr_t window, FFXI::menuwindow_t& info) -> bool
{
    if (window < kPointerFloor || !menus.IsWindowAlive(window))
    {
        return false;
    }

    menus.GetWindowInfo(window, &info);
    return !isAnimating(info);
}

} // namespace

auto MenuManager::CanPress(const uintptr_t window, const int32_t button) const -> bool
{
    FFXI::menuwindow_t info{};
    return GetCurrentWindow() == window && settled(*this, window, info) && button >= 1 && button <= info.Buttons;
}

auto MenuManager::CanCancel(const uintptr_t window) const -> bool
{
    FFXI::menuwindow_t info{};
    const auto         focus = GetCurrentWindow();
    return settled(*this, window, info) && focus != 0 && (focus == window || IsWindowAlive(focus));
}

} // namespace menumanager
