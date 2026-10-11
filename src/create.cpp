#include "menu_manager.h"

#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_title.h>

#include <Ashita.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace menumanager
{

namespace
{

// CTkMenuMng::OnDraw, which loads the manager's address 3 bytes in
constexpr const char* kManagerPattern = "32DBB9????????E8????????85C08944240C743A";
constexpr uint32_t    kManagerOffset  = 3;

// CTkMenuMng::OnKeyDown and CTkMenuCtrlData::SetCursol
constexpr const char* kKeyDownPattern   = "568BF1E8????????84C0753A8BCEE8????????84C07420";
constexpr const char* kSetCursolPattern = "83EC0856578B7C24148BF16685FF7F05BF01000000668B4658";

// CTkHelp::OnDrawPrimitive and CTkTitle::OnDrawPrimitive
constexpr const char* kHelpDrawPattern  = "83EC0853568BF18B0D????????E8????????8B0D????????E8????????A1????????BB808080808D4E20";
constexpr const char* kTitleDrawPattern = "83EC24568BF10FBF864804000083E8000F84";

// CTkTitle::OnDrawPrimitive loads its KaHelpMes with lea ecx, [esi + m_helpMes] this far in. If it isn't there, this
// build keeps the text somewhere else
constexpr uint32_t               kTitleMesLea      = 0xDD;
constexpr std::array<uint8_t, 6> kTitleMesLeaBytes = { 0x8D, 0x8E, offsetof(raw::CTkTitle, m_helpMes), 0, 0, 0 };

auto findTitleDraw(const uintptr_t moduleBase, const uintptr_t moduleSize) -> uintptr_t
{
    const auto             draw = Ashita::Memory::FindPattern(moduleBase, moduleSize, kTitleDrawPattern, 0, 0);
    std::array<uint8_t, 6> lea{};
    if (draw == 0 || !raw::readBytes(lea.data(), draw + kTitleMesLea, lea.size()) || lea != kTitleMesLeaBytes)
    {
        return 0;
    }

    return draw;
}

} // namespace

auto attachMenuManager(const uintptr_t manager, const uintptr_t keyDown, const uintptr_t setCursol, const uintptr_t helpDraw, const uintptr_t titleDraw) -> std::unique_ptr<IMenuManager>
{
    return std::make_unique<MenuManager>(manager, std::bit_cast<KeyDownFn>(keyDown), std::bit_cast<SetCursolFn>(setCursol), helpDraw, titleDraw);
}

auto createMenuManager(const uintptr_t moduleBase, const uintptr_t moduleSize, const char** missing) -> std::unique_ptr<IMenuManager>
{
    const auto report = [&](const char* what)
    {
        if (missing != nullptr)
        {
            *missing = what;
        }
    };
    const auto site    = Ashita::Memory::FindPattern(moduleBase, moduleSize, kManagerPattern, 0, 0);
    uint32_t   manager = 0;
    if (site == 0 || !raw::read(site + kManagerOffset, manager) || manager < raw::kPointerFloor)
    {
        report("menu manager");
        return nullptr;
    }

    const auto keyDown = Ashita::Memory::FindPattern(moduleBase, moduleSize, kKeyDownPattern, 0, 0);
    if (keyDown == 0)
    {
        report("menu key input");
        return nullptr;
    }

    const auto setCursol = Ashita::Memory::FindPattern(moduleBase, moduleSize, kSetCursolPattern, 0, 0);
    if (setCursol == 0)
    {
        report("set cursor");
        return nullptr;
    }

    const auto helpDraw  = Ashita::Memory::FindPattern(moduleBase, moduleSize, kHelpDrawPattern, 0, 0);
    const auto titleDraw = findTitleDraw(moduleBase, moduleSize);
    if (helpDraw == 0)
    {
        report("help line draw");
    }
    else if (titleDraw == 0)
    {
        report("title box draw");
    }
    else
    {
        report(nullptr);
    }

    return attachMenuManager(manager, keyDown, setCursol, helpDraw, titleDraw);
}

} // namespace menumanager
