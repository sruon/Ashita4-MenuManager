#include "fake_window.h"

#include <menumanager/window_watcher.h>

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <cstdint>

using namespace menumanager;
using namespace menumanager::raw;

TEST_CASE("open windows")
{
    FakeWindow      w;
    CTkMenuCtrlData other{};
    CTkNode         otherNode{ .m_pOb = address(other) };
    w.windowNode.m_pNextNode = address(otherNode);
    CHECK(w.menus->GetWindows(nullptr, 0) == 2);

    std::array<uintptr_t, 1> windows{};
    CHECK((w.menus->GetWindows(windows.data(), 1) == 2 && windows[0] == address(w.window)));
    otherNode.m_IsFree = 1;
    CHECK(w.menus->GetWindows(nullptr, 0) == 1);
}

TEST_CASE("windows opening and closing")
{
    FakeWindow    w;
    WindowWatcher watcher;
    watcher.update(*w.menus);
    CHECK((std::ranges::equal(watcher.opened(), std::array<uintptr_t, 1>{ address(w.window) }) && watcher.closed().empty()));
    watcher.update(*w.menus);
    CHECK((watcher.opened().empty() && watcher.closed().empty()));

    CTkMenuCtrlData other{};
    CTkNode         otherNode{ .m_pOb = address(other) };
    w.windowNode.m_pNextNode = address(otherNode);
    w.windowNode.m_IsFree    = 1;
    watcher.update(*w.menus);
    CHECK(std::ranges::equal(watcher.opened(), std::array<uintptr_t, 1>{ address(other) }));
    CHECK(std::ranges::equal(watcher.closed(), std::array<uintptr_t, 1>{ address(w.window) }));
}
