#pragma once

#include <cstdint>

namespace menumanager::FFXI
{

// What the help line draws in place of its text, or over it
enum class MenuHelpBusy : uint8_t
{
    None    = 0,
    Loading = 1, // the "Downloading data" indicator, drawn instead of the text
    Other   = 2, // the auction indicator or one of the other animations
};

// The text of the help line or the title box
struct menuhelp_t
{
    char         Text[256]; // UTF-8, empty when the line shows nothing or Busy is set. Longer text is cut
    MenuHelpBusy Busy;
};

} // namespace menumanager::FFXI
