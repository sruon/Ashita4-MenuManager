#include "fake_window.h"

#include <menumanager/window_state.h>

#include <doctest/doctest.h>

using namespace menumanager;

TEST_CASE("opening and closing animation")
{
    FFXI::menuwindow_t window{};
    CHECK_FALSE(isAnimating(window));
    window.OpenTicks = 1;
    CHECK(isAnimating(window));
    window = { .Status = FFXI::MenuStatus::Closing };
    CHECK(isAnimating(window));

    FakeWindow w; // closing
    w.window.m_MenuCtrlID = 0;
    REQUIRE(w.menus->GetWindowInfo(address(w.window), &window));
    CHECK(isAnimating(window)); // still opening
    w.frame.m_NeckTime = 0;
    REQUIRE(w.menus->GetWindowInfo(address(w.window), &window));
    CHECK_FALSE(isAnimating(window));
}

TEST_CASE("frame fade")
{
    const auto faded = [](const FFXI::MenuFade mode, const int32_t time)
    {
        return opacity({ .FadeMode = mode, .Framed = 1, .FadeTime = time });
    };
    CHECK(opacity({ .FadeMode = FFXI::MenuFade::Hidden }) == 1.0f); // no frame
    CHECK(faded(FFXI::MenuFade::Shown, 5) == 1.0f);
    CHECK(faded(FFXI::MenuFade::Hidden, 5) == 0.0f);
    CHECK((faded(FFXI::MenuFade::Out, 20) == 1.0f && faded(FFXI::MenuFade::Out, 10) == 0.5f && faded(FFXI::MenuFade::Out, 0) == 0.0f));
    CHECK((faded(FFXI::MenuFade::In, 20) == 0.0f && faded(FFXI::MenuFade::In, 5) == 0.75f && faded(FFXI::MenuFade::In, 0) == 1.0f));
    CHECK((faded(FFXI::MenuFade::Out, 30) == 1.0f && faded(FFXI::MenuFade::In, 30) == 0.0f));
}
