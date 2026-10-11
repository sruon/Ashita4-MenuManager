#pragma once

#include <menumanager/ffxi/menu_help.h>
#include <menumanager/ffxi/menu_key.h>
#include <menumanager/ffxi/menu_list.h>
#include <menumanager/ffxi/menu_object.h>
#include <menumanager/ffxi/menu_window.h>

#include <cstdint>
#include <memory>

namespace menumanager
{

// Final Fantasy XI's menu manager. A window is the address of the game's CTkMenuCtrlData
struct IMenuManager
{
    virtual ~IMenuManager() = default;

    // The menu area, as left, top, right, bottom
    virtual bool GetMenuRect(int16_t* rect) const = 0;
    // Writes up to `capacity` open windows, in the manager's order, and returns how many there are
    virtual uint32_t GetWindows(uintptr_t* windows, uint32_t capacity) const = 0;
    // The window with focus, or 0
    virtual uintptr_t GetCurrentWindow() const              = 0;
    virtual bool      IsWindowAlive(uintptr_t window) const = 0;

    virtual bool GetWindowInfo(uintptr_t window, FFXI::menuwindow_t* info) const = 0;
    // Only meaningful for list windows. Returns false when the values don't look like a list
    virtual bool GetWindowList(uintptr_t window, FFXI::menulist_t* list) const = 0;
    // Writes up to `capacity` of the window's draw objects, in draw order, and returns how many there are.
    // With `shapes`, also works out which art file each frame and button draws from.
    // `complete` is set when every read succeeded and the list didn't change during the walk
    virtual uint32_t GetWindowObjects(uintptr_t window, FFXI::menuobject_t* objects, uint32_t capacity, bool shapes, bool* complete) const = 0;
    // The text of the help line (helpwind) or the title box (titlewin). Returns false for any other window, for the
    // title box while it shows the POL handle, and when the window's draw wasn't found
    virtual bool GetWindowHelp(uintptr_t window, FFXI::menuhelp_t* help) const = 0;

    // Whether a key sent now goes to this window and the button is one of its own. The window must be open, have
    // focus, not be opening or closing, and have at least `button` buttons
    virtual bool CanPress(uintptr_t window, int32_t button) const = 0;
    // Whether Cancel can be sent on behalf of this window. It must be open and not opening or closing, and some open
    // window must have focus. That can be a child of this one, which Cancel then closes
    virtual bool CanCancel(uintptr_t window) const = 0;

    virtual void SetWindowCursor(uintptr_t window, int32_t button) = 0;
    // Sends a key to the focused window
    virtual bool PressKey(FFXI::MenuKey key) = 0;
};

// Finds the menu manager in FFXiMain.dll. Returns null if a pattern isn't found, and names it in `missing`.
// The help line and title box draws are optional. Without one the manager is still returned, GetWindowHelp fails for
// that window and `missing` names the draw
auto createMenuManager(uintptr_t moduleBase, uintptr_t moduleSize, const char** missing = nullptr) -> std::unique_ptr<IMenuManager>;

// Uses known addresses instead of searching. keyDown and setCursol can be 0 if PressKey and SetWindowCursor aren't used.
// helpDraw and titleDraw are CTkHelp::OnDrawPrimitive and CTkTitle::OnDrawPrimitive, 0 if GetWindowHelp isn't used
auto attachMenuManager(uintptr_t manager, uintptr_t keyDown, uintptr_t setCursol, uintptr_t helpDraw = 0, uintptr_t titleDraw = 0) -> std::unique_ptr<IMenuManager>;

} // namespace menumanager
