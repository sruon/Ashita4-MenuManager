#include "menu_manager.h"

#include <menumanager/raw/tk_draw_ctrl_frame.h>
#include <menumanager/raw/tk_menu_ctrl_data.h>
#include <menumanager/raw/tk_menu_data.h>
#include <menumanager/raw/walk.h>

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace menumanager
{

using namespace raw;

namespace
{

constexpr size_t kIdPrefixLength = 8; // "menu    "
constexpr size_t kNameLength     = 8;

// "menu    inventor" becomes "inventor"
auto windowName(const uintptr_t window, char (&name)[9]) -> bool
{
    uint32_t                    menuData = 0;
    decltype(CTkMenuData::m_ID) id{};
    if (!read(window + offsetof(CTkMenuCtrlData, m_pMD), menuData) || !read(menuData + offsetof(CTkMenuData, m_ID), id))
    {
        return false;
    }

    std::string_view text(id, std::ranges::find(id, '\0'));
    if (text.size() > kIdPrefixLength)
    {
        text.remove_prefix(kIdPrefixLength);
    }

    text = text.substr(0, std::min(text.find_last_not_of(' ') + 1, kNameLength));
    std::ranges::fill(name, '\0');
    std::ranges::copy(text, name);
    return !text.empty();
}

} // namespace

auto MenuManager::GetWindowInfo(const uintptr_t window, FFXI::menuwindow_t* info) const -> bool
{
    if (info == nullptr || window < kPointerFloor)
    {
        return false;
    }

    *info           = {};
    info->Address   = window;
    const bool name = windowName(window, info->Name);
    // read field by field, so one bad read doesn't lose the rest
    const bool rect = read(window + offsetof(CTkMenuCtrlData, m_WinRect), info->Rect);
    read(window + offsetof(CTkMenuCtrlData, m_CursolNum), info->Cursor);
    read(window + offsetof(CTkMenuCtrlData, m_ButtonSuu), info->Buttons);
    read(window + offsetof(CTkMenuCtrlData, m_MenuCtrlID), info->Status);
    read(window + offsetof(CTkMenuCtrlData, m_Priority), info->Priority);
    read(window + offsetof(CTkMenuCtrlData, m_EnableCursolDraw), info->CursorDraw);

    // the opening animation and fade belong to the window's first frame
    uint32_t head = 0;
    read(window + offsetof(CTkMenuCtrlData, m_DrawShapes), head);
    forEachNode(head, kMaxObjects, [&](const uint32_t object)
                {
                    auto type = static_cast<FFXI::MenuObjectType>(-1);
                    if (!read(object + offsetof(CTkDrawCtrlFrame, m_RecKind), type) || type != FFXI::MenuObjectType::Frame)
                    {
                        return true;
                    }

                    read(object + offsetof(CTkDrawCtrlFrame, m_NeckTime), info->OpenTicks);
                    info->Framed = read(object + offsetof(CTkDrawCtrlFrame, m_fadeMode), info->FadeMode) && read(object + offsetof(CTkDrawCtrlFrame, m_fadeTime), info->FadeTime);
                    return false;
                });
    return name && rect;
}

} // namespace menumanager
