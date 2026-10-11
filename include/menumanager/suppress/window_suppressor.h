#pragma once

#include <menumanager/suppress/draw_listener.h>
#include <menumanager/suppress/screen_box.h>

#include <d3d8.h>

#include <array>
#include <cstdint>
#include <memory>

namespace menumanager
{

// CTkMenuCtrlData::OnDraw, __thiscall, called through __fastcall
using WindowDrawFn = void(__fastcall*)(uintptr_t window, void* edx);

// CTkMenuCtrlData::OnDraw, for a plugin that searches for it itself
constexpr const char* kWindowDrawPattern = "83EC24535556578BE9E8????????668B455633FF663BC77E05";

// Hides the game's own drawing of the windows a DrawListener picks. It hooks CTkMenuCtrlData::OnDraw, so a hidden
// window still runs its logic, and drops the window's Direct3D draws as the plugin forwards them from its draw
// callbacks. Only one can hold the hook at a time
class WindowSuppressor
{
public:
    // Hooks OnDraw at onDraw. With 0 nothing is hooked, and drawWindow runs only when called directly
    explicit WindowSuppressor(uintptr_t onDraw = 0);
    ~WindowSuppressor();
    WindowSuppressor(const WindowSuppressor&)                    = delete;
    auto operator=(const WindowSuppressor&) -> WindowSuppressor& = delete;

    auto hooked() const -> bool;
    // Removes the hook. Returns false while it is still in place because the write failed or another hook was put on
    // top of it. The hook then passes every draw to the game, but the module must stay loaded
    auto unhook() -> bool;

    // null hides nothing
    auto setListener(DrawListener* listener) -> void;
    // A hidden window's DrawPrimitiveUP draws that land well outside its rect still draw. The target arrow is one. On by
    // default
    auto setKeepOutsideDraws(bool keep) -> void;

    // The hook's body for one window draw. draw is the game's OnDraw
    auto drawWindow(uintptr_t window, WindowDrawFn draw, void* edx) -> void;

    // From the plugin's Direct3D draw callbacks. True drops the draw. Send DrawIndexedPrimitive to onDrawPrimitive
    auto onDrawPrimitive(IDirect3DDevice8* device, D3DPRIMITIVETYPE type, uint32_t count) const -> bool;
    auto onDrawPrimitiveUP(IDirect3DDevice8* device, D3DPRIMITIVETYPE type, uint32_t count, const void* vertices, uint32_t stride) const -> bool;
    auto onDrawIndexedPrimitiveUP(IDirect3DDevice8* device, D3DPRIMITIVETYPE type, uint32_t minIndex, uint32_t vertexCount, uint32_t count, const void* vertices, uint32_t stride) const -> bool;

private:
    // A window that is drawing
    struct Frame
    {
        uintptr_t     window;
        DrawListener* listener;
        bool          hidden;
        ScreenBox     bounds; // its rect with a margin
    };

    // Deeper window draws count as part of the one that holds them
    static constexpr uint32_t kMaxDepth = 8;

    auto current() const -> const Frame*;

    DrawListener*                listener_{};
    bool                         keepOutside_ = true;
    std::array<Frame, kMaxDepth> frames_{};
    uint32_t                     depth_{};
    uint8_t*                     target_{};
    std::array<uint8_t, 5>       patch_{}; // the prologue as the hook left it
};

// Finds CTkMenuCtrlData::OnDraw and hooks it. Returns null if either fails, and names which in `missing`
auto createWindowSuppressor(uintptr_t moduleBase, uintptr_t moduleSize, const char** missing = nullptr) -> std::unique_ptr<WindowSuppressor>;
// Hooks a known OnDraw. Returns null if the hook can't be installed
auto attachWindowSuppressor(uintptr_t onDraw) -> std::unique_ptr<WindowSuppressor>;

} // namespace menumanager
