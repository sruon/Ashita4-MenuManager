#pragma once

#include <menumanager/suppress/screen_box.h>

#include <d3d8.h>

#include <cstdint>

namespace menumanager
{

// Added around a window's rect. The inventory's counter sits 28 px above it and the target arrow hundreds away
constexpr float kFarMargin = 48;

// How many vertices a draw of this many primitives uses
auto vertexCount(D3DPRIMITIVETYPE type, uint32_t primitives) -> uint32_t;
// The device's vertex shader, 0 if it can't be read
auto vertexShader(IDirect3DDevice8* device) -> uint32_t;
// The box covered by `count` vertices from `first`. Only pre-transformed (XYZRHW) vertices have one
auto drawBox(uint32_t shader, const void* vertices, uint32_t first, uint32_t count, uint32_t stride) -> ScreenBox;
// The window's rect plus kFarMargin, or the margin around 0 if the rect can't be read
auto windowBounds(uintptr_t window) -> ScreenBox;
// Whether a draw lands clear of a window's bounds
auto outside(const ScreenBox& box, const ScreenBox& bounds) -> bool;

} // namespace menumanager
