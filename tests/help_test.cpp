#include "fake_window.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstring>
#include <ostream>
#include <string_view>

using namespace menumanager;

namespace
{

// The help line as the client leaves it, showing level 1 with the scroll padding after the message
auto showHelp(FakeWindow& w, const std::string_view text) -> void
{
    w.window.m_pMP   = address(w.help);
    w.help.m_helpMes = { .m_pDisp = 1, .m_mesW = 900, .m_dispLevel = 1 };
    std::ranges::copy(text, w.help.m_helpMes.levels[1]);
}

} // namespace

TEST_CASE("help line text")
{
    FakeWindow w;
    showHelp(w, "Hide chat messages.  ");
    FFXI::menuhelp_t help{};
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK(std::string_view(help.Text) == "Hide chat messages.");
    CHECK(help.Busy == FFXI::MenuHelpBusy::None);

    w.help.m_helpMes.m_pDisp = 0; // nothing shown
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK(std::string_view(help.Text).empty());

    w.help.m_helpMes.m_pDisp     = 1;
    w.help.m_helpMes.m_dispLevel = 4;
    CHECK_FALSE(w.menus->GetWindowHelp(address(w.window), &help));
}

TEST_CASE("help line indicators")
{
    FakeWindow w;
    showHelp(w, "Hide chat messages.");
    FFXI::menuhelp_t help{};
    w.help.m_helpMes.m_loadBit = 0x40;
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK((help.Busy == FFXI::MenuHelpBusy::Loading && std::string_view(help.Text).empty()));

    w.help.m_helpMes.anims[0] = 1;
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK(help.Busy == FFXI::MenuHelpBusy::Other);

    w.help.m_helpMes.anims[0]  = 0;
    w.help.m_helpMes.m_loadBit = 0x80000000; // the auction indicator
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK(help.Busy == FFXI::MenuHelpBusy::Other);
}

TEST_CASE("title box text")
{
    FakeWindow w;
    w.window.m_pMP    = address(w.title);
    w.title.m_helpMes = { .m_pDisp = 1, .m_mesW = 100, .m_dispLevel = 0 };
    std::ranges::copy(std::string_view("Items"), w.title.m_helpMes.levels[0]);
    FFXI::menuhelp_t help{};
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK(std::string_view(help.Text) == "Items");

    w.title.m_drawmode = 1; // the POL handle
    CHECK_FALSE(w.menus->GetWindowHelp(address(w.window), &help));
}

TEST_CASE("windows without help text")
{
    FakeWindow       w;
    FFXI::menuhelp_t help{};
    CHECK_FALSE(w.menus->GetWindowHelp(address(w.window), &help)); // no window class

    showHelp(w, "Hide chat messages.");
    w.helpVtable[4] = 0x520000; // some other class
    CHECK_FALSE(w.menus->GetWindowHelp(address(w.window), &help));

    w.helpVtable[4]    = kHelpDraw;
    const auto unfound = attachMenuManager(address(w.manager), 0, 0);
    CHECK_FALSE(unfound->GetWindowHelp(address(w.window), &help));
    CHECK_FALSE(w.menus->GetWindowHelp(address(w.window), nullptr));
}

TEST_CASE("long help text is cut between characters")
{
    FakeWindow w;
    showHelp(w, {});
    auto& level = w.help.m_helpMes.levels[1];
    for (size_t i = 0; i + 1 < sizeof(level) - 1; i += 2)
    {
        level[i]     = '\x83'; // katakana a, three bytes in UTF-8
        level[i + 1] = '\x41';
    }

    FFXI::menuhelp_t help{};
    REQUIRE(w.menus->GetWindowHelp(address(w.window), &help));
    CHECK(std::strlen(help.Text) == 255);
    CHECK(std::string_view(help.Text).ends_with("\xE3\x82\xA2"));
}
