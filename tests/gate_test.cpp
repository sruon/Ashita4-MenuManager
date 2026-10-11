#include "fake_window.h"

#include <doctest/doctest.h>
#include <windows.h>

#include <bit>
#include <limits>
#include <utility>

using namespace menumanager;
using namespace menumanager::raw;

namespace
{

// Open, done opening, with five buttons
auto settle(FakeWindow& w) -> uintptr_t
{
    w.window.m_MenuCtrlID = 0;
    w.window.m_ButtonSuu  = 5;
    w.frame.m_NeckTime    = 0;
    return address(w.window);
}

} // namespace

TEST_CASE("pressing a window's buttons")
{
    FakeWindow  w;
    const auto  window = settle(w);
    const auto& menus  = *w.menus;
    CHECK((menus.CanPress(window, 1) && menus.CanPress(window, 5)));
    CHECK_FALSE((menus.CanPress(window, 0) || menus.CanPress(window, -1) || menus.CanPress(window, 6) || menus.CanPress(window, (std::numeric_limits<int32_t>::max)())));

    w.frame.m_NeckTime = 3; // opening
    CHECK_FALSE(menus.CanPress(window, 1));
    w.frame.m_NeckTime    = 0;
    w.window.m_MenuCtrlID = std::to_underlying(FFXI::MenuStatus::Closing);
    CHECK_FALSE(menus.CanPress(window, 1));
    w.window.m_MenuCtrlID = 0;
    w.manager.m_pCurMCD   = 0;
    CHECK_FALSE(menus.CanPress(window, 1));
    w.manager.m_pCurMCD   = window;
    w.windowNode.m_IsFree = 1; // removed while it still had focus
    CHECK_FALSE(menus.CanPress(window, 1));
    CHECK_FALSE(menus.CanPress(0, 1));
}

TEST_CASE("cancelling for a window")
{
    FakeWindow      w;
    const auto      window = settle(w);
    const auto&     menus  = *w.menus;
    CTkMenuCtrlData child{};
    CTkNode         childNode{ .m_pOb = address(child) };
    w.windowNode.m_pNextNode = address(childNode);
    w.manager.m_pCurMCD      = address(child);
    CHECK(menus.CanCancel(window)); // closes the focused child
    CHECK_FALSE(menus.CanPress(window, 1));

    childNode.m_IsFree = 1;
    CHECK_FALSE(menus.CanCancel(window));
    w.manager.m_pCurMCD = 0;
    CHECK_FALSE(menus.CanCancel(window));

    w.manager.m_pCurMCD  = window;
    w.window.m_ButtonSuu = 0; // a dialog with nothing to press can still be cancelled
    CHECK((menus.CanCancel(window) && !menus.CanPress(window, 1)));
    w.window.m_MenuCtrlID = std::to_underlying(FFXI::MenuStatus::Closing);
    CHECK_FALSE(menus.CanCancel(window));
}

TEST_CASE("a window is alive while a live node holds it")
{
    FakeWindow  w;
    const auto  window = address(w.window);
    const auto& menus  = *w.menus;
    CTkNode     removed{ .m_pNextNode = address(w.windowNode), .m_pOb = window, .m_IsFree = 1 };
    w.manager.m_DrawDatas.m_pTopNode = address(removed);
    CHECK(menus.IsWindowAlive(window)); // a removed node doesn't hide the live one after it

    w.windowNode.m_IsFree = 1;
    CHECK_FALSE(menus.IsWindowAlive(window));
    w.windowNode.m_pNextNode = address(removed); // a loop of removed nodes still ends
    CHECK_FALSE(menus.IsWindowAlive(window));
    CHECK_FALSE((menus.IsWindowAlive(0) || menus.IsWindowAlive(0xFFFF)));

    void* page = VirtualAlloc(nullptr, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS);
    REQUIRE(page != nullptr);
    w.manager.m_DrawDatas.m_pTopNode = static_cast<uint32_t>(std::bit_cast<uintptr_t>(page));
    CHECK_FALSE(menus.IsWindowAlive(window));
    VirtualFree(page, 0, MEM_RELEASE);
}
