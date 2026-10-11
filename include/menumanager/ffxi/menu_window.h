#pragma once

#include <menumanager/ffxi/menu_fade.h>

#include <cstdint>

namespace menumanager::FFXI
{

// Only Closing is known
enum class MenuStatus : uint32_t
{
    Closing = 0xE,
};

struct menuwindow_t
{
    uintptr_t  Address; // the window handle
    char       Name[9]; // the menu's id, like "inventor" or "equip"
    int16_t    Rect[4]; // left, top, right, bottom, in menu pixels
    int16_t    Cursor;  // the selected button
    int16_t    Buttons;
    MenuStatus Status;
    uint8_t    Priority;   // draw order
    uint8_t    CursorDraw; // 0 while the game hides the cursor, such as when a field takes text
    int16_t    OpenTicks;  // frames left of the opening animation, 0 once open
    MenuFade   FadeMode;
    uint8_t    Framed;   // FadeMode and FadeTime could be read
    int32_t    FadeTime; // frames left of the fade
};

} // namespace menumanager::FFXI
