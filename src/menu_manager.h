#pragma once

#include <menumanager/menu_manager.h>

#include <cstdint>

namespace menumanager
{

// CTkMenuMng::OnKeyDown and CTkMenuCtrlData::SetCursol, both __thiscall, called through __fastcall
using KeyDownFn   = int32_t(__fastcall*)(uintptr_t manager, void* edx, int32_t key);
using SetCursolFn = void(__fastcall*)(uintptr_t window, void* edx, int32_t button, int32_t moveMouse);

// Walk limits, so a corrupt or circular list can't hang. The busiest window seen draws about 90 objects (keypadus)
constexpr int32_t kMaxWindows = 512;
constexpr int32_t kMaxObjects = 256;

class MenuManager final : public IMenuManager
{
public:
    MenuManager(uintptr_t manager, KeyDownFn keyDown, SetCursolFn setCursol, uintptr_t helpDraw, uintptr_t titleDraw);

    auto GetMenuRect(int16_t* rect) const -> bool override;
    auto GetWindows(uintptr_t* windows, uint32_t capacity) const -> uint32_t override;
    auto GetCurrentWindow() const -> uintptr_t override;
    auto IsWindowAlive(uintptr_t window) const -> bool override;
    auto GetWindowInfo(uintptr_t window, FFXI::menuwindow_t* info) const -> bool override;
    auto GetWindowList(uintptr_t window, FFXI::menulist_t* list) const -> bool override;
    auto GetWindowObjects(uintptr_t window, FFXI::menuobject_t* objects, uint32_t capacity, bool shapes, bool* complete) const -> uint32_t override;
    auto GetWindowHelp(uintptr_t window, FFXI::menuhelp_t* help) const -> bool override;
    auto CanPress(uintptr_t window, int32_t button) const -> bool override;
    auto CanCancel(uintptr_t window) const -> bool override;
    auto SetWindowCursor(uintptr_t window, int32_t button) -> void override;
    auto PressKey(FFXI::MenuKey key) -> bool override;

private:
    uintptr_t   manager_;
    KeyDownFn   keyDown_;
    SetCursolFn setCursol_;
    uintptr_t   helpDraw_;  // CTkHelp::OnDrawPrimitive
    uintptr_t   titleDraw_; // CTkTitle::OnDrawPrimitive
};

} // namespace menumanager
