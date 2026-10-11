#include "fake_window.h"
#include "suppress/draw_box.h"

#include <menumanager/suppress/draw_call.h>
#include <menumanager/suppress/draw_listener.h>
#include <menumanager/suppress/screen_box.h>
#include <menumanager/suppress/window_suppressor.h>

#include <d3d8.h>
#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

using namespace menumanager;

namespace
{

// Hides the windows in `hide` and writes down every call
class Recorder final : public DrawListener
{
public:
    std::vector<uintptr_t>   hide;
    std::vector<std::string> calls;

    auto beginWindow(const uintptr_t window) -> bool override
    {
        calls.push_back("begin " + std::to_string(window));
        return std::ranges::find(hide, window) != hide.end();
    }

    auto endWindow(const uintptr_t window, const bool hidden) -> void override
    {
        calls.push_back("end " + std::to_string(window) + (hidden ? " hidden" : ""));
    }

    auto draw(const uintptr_t window, const DrawCall& call) -> void override
    {
        calls.push_back("draw " + std::to_string(window) + " " + std::to_string(call.count));
    }
};

// The game's OnDraw. edx carries what it draws
void __fastcall fakeDraw(uintptr_t, void* edx)
{
    (*static_cast<std::function<void()>*>(edx))();
}

auto drawWith(WindowSuppressor& suppressor, const uintptr_t window, std::function<void()> body) -> void
{
    suppressor.drawWindow(window, &fakeDraw, &body);
}

} // namespace

TEST_CASE("a hidden window's draws are dropped")
{
    FakeWindow       w;
    const auto       window = address(w.window);
    Recorder         recorder;
    WindowSuppressor suppressor;
    recorder.hide = { window };
    suppressor.setListener(&recorder);

    bool dropped = false;
    drawWith(suppressor, window, [&]
             {
                 dropped = suppressor.onDrawPrimitive(nullptr, D3DPT_TRIANGLELIST, 2);
             });
    CHECK(dropped);
    CHECK_FALSE(suppressor.onDrawPrimitive(nullptr, D3DPT_TRIANGLELIST, 2));
    CHECK(recorder.calls == std::vector<std::string>{ "begin " + std::to_string(window), "draw " + std::to_string(window) + " 2", "end " + std::to_string(window) + " hidden" });
}

TEST_CASE("a shown window's draws are kept")
{
    FakeWindow       w;
    const auto       window = address(w.window);
    Recorder         recorder;
    WindowSuppressor suppressor;
    suppressor.setListener(&recorder);

    bool dropped = true;
    drawWith(suppressor, window, [&]
             {
                 dropped = suppressor.onDrawPrimitiveUP(nullptr, D3DPT_TRIANGLESTRIP, 2, nullptr, 28);
             });
    CHECK_FALSE(dropped);
    CHECK(recorder.calls.back() == "end " + std::to_string(window));
}

TEST_CASE("without a listener every draw goes through")
{
    FakeWindow       w;
    WindowSuppressor suppressor;
    bool             drew    = false;
    bool             dropped = true;
    drawWith(suppressor, address(w.window), [&]
             {
                 drew    = true;
                 dropped = suppressor.onDrawIndexedPrimitiveUP(nullptr, D3DPT_TRIANGLELIST, 0, 4, 2, nullptr, 28);
             });
    CHECK(drew);
    CHECK_FALSE(dropped);
    CHECK_FALSE(suppressor.hooked());
}

TEST_CASE("a window drawn inside another has its own verdict, and the outer one's comes back after")
{
    FakeWindow       w;
    FakeWindow       other;
    const auto       outer = address(w.window);
    const auto       inner = address(other.window);
    Recorder         recorder;
    WindowSuppressor suppressor;
    recorder.hide = { outer };
    suppressor.setListener(&recorder);

    std::vector<bool> dropped;
    drawWith(suppressor, outer, [&]
             {
                 dropped.push_back(suppressor.onDrawPrimitive(nullptr, D3DPT_TRIANGLELIST, 1));
                 drawWith(suppressor, inner, [&]
                          {
                              dropped.push_back(suppressor.onDrawPrimitive(nullptr, D3DPT_TRIANGLELIST, 2));
                          });
                 dropped.push_back(suppressor.onDrawPrimitive(nullptr, D3DPT_TRIANGLELIST, 3));
             });
    CHECK(dropped == std::vector<bool>{ true, false, true });
    const auto o = std::to_string(outer);
    const auto i = std::to_string(inner);
    CHECK(recorder.calls == std::vector<std::string>{ "begin " + o, "draw " + o + " 1", "begin " + i, "draw " + i + " 2", "end " + i, "draw " + o + " 3", "end " + o + " hidden" });
}

TEST_CASE("a pre-transformed draw's box")
{
    struct Vertex
    {
        float    x, y, z, rhw;
        uint32_t diffuse;
        float    u, v;
    };

    const std::array<Vertex, 4> quad{ Vertex{ .x = 10, .y = 20 }, Vertex{ .x = 50, .y = 20 }, Vertex{ .x = 10, .y = 60 }, Vertex{ .x = 50, .y = 60 } };
    constexpr uint32_t          kFvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    const auto box = drawBox(kFvf, quad.data(), 0, vertexCount(D3DPT_TRIANGLESTRIP, 2), sizeof(Vertex));
    CHECK((box.left == 10 && box.top == 20 && box.right == 50 && box.bottom == 60));
    const auto tail = drawBox(kFvf, quad.data(), 2, 2, sizeof(Vertex));
    CHECK((tail.left == 10 && tail.top == 60 && tail.right == 50 && tail.bottom == 60));

    CHECK_FALSE(drawBox(D3DFVF_XYZ | D3DFVF_DIFFUSE, quad.data(), 0, 4, sizeof(Vertex)).valid());
    CHECK_FALSE(drawBox(kFvf, quad.data(), 0, 4, 12).valid());
    CHECK_FALSE(drawBox(kFvf, nullptr, 0, 4, sizeof(Vertex)).valid());
    CHECK_FALSE(drawBox(0x10000, quad.data(), 0, 4, sizeof(Vertex)).valid());
}

TEST_CASE("draws well outside a window's rect")
{
    FakeWindow w;
    w.window.m_WinRect[0] = 100;
    w.window.m_WinRect[1] = 200;
    w.window.m_WinRect[2] = 300;
    w.window.m_WinRect[3] = 400;
    const auto bounds     = windowBounds(address(w.window));
    CHECK((bounds.left == 100 - kFarMargin && bounds.top == 200 - kFarMargin && bounds.right == 300 + kFarMargin && bounds.bottom == 400 + kFarMargin));

    CHECK_FALSE(outside({ 120, 220, 140, 240 }, bounds));
    CHECK_FALSE(outside({ 40, 220, 52, 240 }, bounds)); // touches the margin
    CHECK(outside({ 10, 220, 51, 240 }, bounds));
    CHECK(outside({ 120, 500, 140, 520 }, bounds));
    CHECK_FALSE(outside(ScreenBox::none(), bounds)); // a box that isn't known stays with the window
}
