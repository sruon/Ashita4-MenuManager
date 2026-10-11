#include "fake_window.h"

#include <doctest/doctest.h>

#include <ostream>
#include <string_view>

using namespace menumanager;

TEST_CASE("a window's frame, button and cursor")
{
    FakeWindow w;
    const auto [objects, complete] = w.objects();
    REQUIRE(complete);
    REQUIRE(objects.size() == 3);
    CHECK(objects[0].Type == FFXI::MenuObjectType::Frame);
    CHECK((objects[1].Type == FFXI::MenuObjectType::Button && objects[1].Number == 7 && objects[1].Status == 5));
    CHECK((objects[1].X == 12 && objects[1].Y == 34 && objects[1].Width == 56 && objects[1].Height == 78));
    CHECK((objects[2].Type == FFXI::MenuObjectType::Cursor && objects[2].X == 9 && objects[2].Y == 37));
}

TEST_CASE("a window's name, status and fade")
{
    FakeWindow         w;
    FFXI::menuwindow_t info{};
    REQUIRE(w.menus->GetWindowInfo(address(w.window), &info));
    CHECK(std::string_view(info.Name) == "inventor");
    CHECK(info.Status == FFXI::MenuStatus::Closing);
    CHECK((info.OpenTicks == 11 && info.Framed && info.FadeMode == FFXI::MenuFade::In && info.FadeTime == 27));
}
