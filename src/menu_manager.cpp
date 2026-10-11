#include "menu_manager.h"

#include <menumanager/raw/tk_menu_mng.h>
#include <menumanager/raw/walk.h>

#include <cstddef>
#include <utility>

namespace menumanager
{

using namespace raw;

MenuManager::MenuManager(const uintptr_t manager, const KeyDownFn keyDown, const SetCursolFn setCursol, const uintptr_t helpDraw, const uintptr_t titleDraw)
: manager_(manager)
, keyDown_(keyDown)
, setCursol_(setCursol)
, helpDraw_(helpDraw)
, titleDraw_(titleDraw)
{
}

auto MenuManager::GetMenuRect(int16_t* rect) const -> bool
{
    return readBytes(rect, manager_ + offsetof(CTkMenuMng, m_WinRect), sizeof(CTkMenuMng::m_WinRect));
}

auto MenuManager::GetCurrentWindow() const -> uintptr_t
{
    uint32_t current = 0;
    return read(manager_ + offsetof(CTkMenuMng, m_pCurMCD), current) ? current : 0;
}

auto MenuManager::IsWindowAlive(const uintptr_t window) const -> bool
{
    uint32_t head  = 0;
    bool     alive = false;
    if (read(manager_ + offsetof(CTkMenuMng, m_DrawDatas), head))
    {
        forEachNode(head, kMaxWindows, [&](const uint32_t other)
                    {
                        alive = other == window;
                        return !alive;
                    });
    }

    return alive;
}

auto MenuManager::SetWindowCursor(const uintptr_t window, const int32_t button) -> void
{
    if (window >= kPointerFloor)
    {
        setCursol_(window, nullptr, button, 0);
    }
}

auto MenuManager::PressKey(const FFXI::MenuKey key) -> bool
{
    return keyDown_(manager_, nullptr, std::to_underlying(key)) != 0;
}

} // namespace menumanager
