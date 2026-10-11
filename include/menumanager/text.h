#pragma once

#include <string>
#include <string_view>

namespace menumanager
{

// Converts the game's text to UTF-8. It is Shift-JIS (code page 932) with the client's control codes mixed in. Colour
// codes are dropped, auto-translate brackets become { and }, and an auto-translate phrase becomes [AT:xxxxxxxx] with its
// four id bytes in hex. Stops at the first NUL
auto decodeText(std::string_view cp932) -> std::string;
// The same for a NUL-terminated string. Null reads as empty
auto decodeText(const char* cp932) -> std::string;

} // namespace menumanager
