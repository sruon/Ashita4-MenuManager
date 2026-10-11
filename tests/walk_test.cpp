#include "fake_window.h"
#include "menu_manager.h"

#include <doctest/doctest.h>

#include <array>

using namespace menumanager::raw;

TEST_CASE("removed nodes and loops")
{
    FakeWindow w;
    w.buttonNode.m_IsFree = 1; // a removed node is skipped, even with a bad pointer
    w.buttonNode.m_pOb    = 1;
    CHECK((w.objects().second && w.objects().first.size() == 2));

    w.buttonNode.m_IsFree = 0; // the same node live: incomplete
    CHECK_FALSE(w.objects().second);

    w.buttonNode.m_pOb       = address(w.button);
    w.buttonNode.m_pNextNode = address(w.frameNode); // a loop
    CHECK_FALSE(w.objects().second);
}

TEST_CASE("the length limit")
{
    FakeWindow                                        w;
    std::array<CTkNode, menumanager::kMaxObjects + 1> chain{}; // one more than the limit
    for (size_t i = 0; i + 1 < chain.size(); ++i)
    {
        chain[i] = { .m_pNextNode = address(chain[i + 1]), .m_pOb = address(w.cursor) };
    }

    w.window.m_DrawShapes.m_pTopNode = address(chain[0]);
    CHECK_FALSE(w.objects().second);
    chain[menumanager::kMaxObjects - 1].m_pNextNode = 0;
    CHECK(w.objects().second);
}

TEST_CASE("no window")
{
    FakeWindow w;
    CHECK(w.menus->GetWindowObjects(0, nullptr, 0, false, nullptr) == 0);
}
