#include "fake_window.h"

#include <doctest/doctest.h>

#include <ostream>
#include <string_view>

TEST_CASE("the art a button draws")
{
    FakeWindow w;
    auto       button = w.objects().first[1];
    CHECK((button.Matched && button.Index == 1 && button.Shape == w.shapes[1] && std::string_view(button.Library, sizeof(button.Library)) == kArtFile));

    w.button.m_pUserShape = w.shapes[1];
    button                = w.objects().first[1];
    CHECK((button.Overridden && !button.Matched));

    w.button.m_pUserShape = 0;
    w.shapes[0]           = w.shapes[1]; // two shapes at the same address: can't tell which one is drawn
    CHECK_FALSE(w.objects().first[1].Matched);
}

TEST_CASE("bad art data fails the walk")
{
    FakeWindow w;
    w.resources[1].m_RscNum = 2; // past the end of the file
    CHECK_FALSE(w.objects().second);
    w.resources[1].m_RscNum = -1;
    CHECK_FALSE(w.objects().second);
    w.resources[1].m_RscNum    = 1;
    w.resources[1].m_RscID[15] = 'X'; // the file's id doesn't match
    CHECK_FALSE(w.objects().second);
}
