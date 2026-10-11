#include "fake_window.h"

#include <doctest/doctest.h>
#include <windows.h>

#include <bit>

TEST_CASE("unreadable memory")
{
    FakeWindow w;
    void*      page = VirtualAlloc(nullptr, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS);
    REQUIRE(page != nullptr);

    w.resources[1].m_Shapes        = static_cast<uint32_t>(std::bit_cast<uintptr_t>(page));
    const auto [objects, complete] = w.objects();
    CHECK_FALSE(complete);
    CHECK(objects.size() == 3);     // the objects themselves were still read
    CHECK(w.objects(false).second); // without art, the bad pointer is never followed

    w.resources[1].m_Shapes = address(w.file);
    CHECK(w.objects().second); // and the next clean read is complete again
    VirtualFree(page, 0, MEM_RELEASE);
}
