#pragma once

#include <cstdint>

namespace menumanager::FFXI
{

enum class MenuObjectType : int16_t
{
    Frame  = 0,
    Button = 1,
    Cursor = 2,
};

// One thing a window draws: its frame, a button or the cursor
struct menuobject_t
{
    uintptr_t      Address;
    MenuObjectType Type;
    int16_t        Number; // buttons: the button's number on the current page
    int16_t        X;      // buttons and the cursor: position, in menu pixels
    int16_t        Y;
    int16_t        Width; // buttons only
    int16_t        Height;
    uint32_t       Status;      // buttons only
    uintptr_t      Shape;       // the art being drawn, 0 if none
    char           Library[16]; // with Matched: the art file it comes from, like "menu    windowps"
    int16_t        Index;       // with Matched: the art's index in that file, otherwise -1
    uint8_t        Matched;     // the art was traced back to exactly one file and index
    uint8_t        Overridden;  // the button draws replacement art instead (window style settings)
};

} // namespace menumanager::FFXI
