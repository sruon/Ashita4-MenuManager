#include "menu_manager.h"

#include <menumanager/raw/ka_help_mes.h>
#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_help.h>
#include <menumanager/raw/tk_menu_ctrl_data.h>
#include <menumanager/raw/tk_title.h>
#include <menumanager/text.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>

namespace menumanager
{

using namespace raw;

namespace
{

// The vtable slot of the window class's OnDrawPrimitive
constexpr uint32_t kOnDrawPrimitive = 0x10;
constexpr uint32_t kAuctionBit      = 0x80000000;

auto busy(const KaHelpMes& mes) -> FFXI::MenuHelpBusy
{
    if (mes.anims[0] != 0 || mes.anims[1] != 0 || (mes.m_loadBit & kAuctionBit) != 0)
    {
        return FFXI::MenuHelpBusy::Other;
    }

    return mes.m_loadBit != 0 ? FFXI::MenuHelpBusy::Loading : FFXI::MenuHelpBusy::None;
}

// Copies as much as fits without splitting a UTF-8 sequence
auto copyText(const std::string_view utf8, char (&text)[256]) -> void
{
    auto length = std::min(utf8.size(), std::size(text) - 1);
    while (length > 0 && length < utf8.size() && (static_cast<uint8_t>(utf8[length]) & 0xC0) == 0x80)
    {
        --length;
    }

    std::ranges::copy(utf8.substr(0, length), text);
    text[length] = '\0';
}

} // namespace

auto MenuManager::GetWindowHelp(const uintptr_t window, FFXI::menuhelp_t* help) const -> bool
{
    uint32_t primitive = 0, vtable = 0, draw = 0;
    if (help == nullptr || window < kPointerFloor || !read(window + offsetof(CTkMenuCtrlData, m_pMP), primitive) || !read(primitive, vtable) ||
        !read(vtable + kOnDrawPrimitive, draw) || draw == 0)
    {
        return false;
    }

    KaHelpMes mes{};
    if (CTkHelp line{}; draw == helpDraw_ && read(primitive, line))
    {
        mes = line.m_helpMes;
    }
    else if (CTkTitle title{}; draw == titleDraw_ && read(primitive, title) && title.m_drawmode == 0)
    {
        mes = title.m_helpMes;
    }
    else
    {
        return false;
    }

    *help      = {};
    help->Busy = busy(mes);
    if (help->Busy != FFXI::MenuHelpBusy::None || mes.m_pDisp == 0 || mes.m_mesW == 0)
    {
        return true;
    }

    if (mes.m_dispLevel < 0 || mes.m_dispLevel >= std::ssize(mes.levels))
    {
        return false;
    }

    // the game pads scrolling text with trailing spaces
    std::string_view text(mes.levels[mes.m_dispLevel], sizeof(mes.levels[0]) - 1);
    text = text.substr(0, text.find('\0'));
    text = text.substr(0, text.find_last_not_of(' ') + 1);
    copyText(decodeText(text), help->Text);
    return true;
}

} // namespace menumanager
