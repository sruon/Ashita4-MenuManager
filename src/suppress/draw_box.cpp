#include "draw_box.h"

#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_menu_ctrl_data.h>
#include <menumanager/suppress/screen_box.h>

#include <d3d8.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace menumanager
{

namespace
{

constexpr uint32_t kPositionBytes = 16;     // x, y, z, rhw
constexpr uint32_t kMaxFvf        = 0xFFFF; // anything above is a shader handle, not a vertex format
constexpr uint32_t kMaxVertices   = 4096;

} // namespace

auto vertexCount(const D3DPRIMITIVETYPE type, const uint32_t primitives) -> uint32_t
{
    switch (type)
    {
        case D3DPT_TRIANGLELIST:
            return primitives * 3;
        case D3DPT_TRIANGLESTRIP:
        case D3DPT_TRIANGLEFAN:
            return primitives + 2;
        case D3DPT_LINELIST:
            return primitives * 2;
        case D3DPT_LINESTRIP:
            return primitives + 1;
        default:
            return primitives;
    }
}

auto vertexShader(IDirect3DDevice8* device) -> uint32_t
{
    DWORD shader = 0;
    if (device == nullptr || FAILED(device->GetVertexShader(&shader)))
    {
        return 0;
    }

    return shader;
}

auto drawBox(const uint32_t shader, const void* vertices, const uint32_t first, const uint32_t count, const uint32_t stride) -> ScreenBox
{
    auto box = ScreenBox::none();
    if (vertices == nullptr || stride < kPositionBytes || shader > kMaxFvf || (shader & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW)
    {
        return box;
    }

    const auto start = reinterpret_cast<uintptr_t>(vertices) + static_cast<size_t>(first) * stride;
    for (uint32_t index = 0; index < count && index < kMaxVertices; ++index)
    {
        std::array<float, 2> position{};
        if (!raw::read(start + static_cast<size_t>(index) * stride, position))
        {
            break;
        }

        box.left   = (std::min)(box.left, position[0]);
        box.top    = (std::min)(box.top, position[1]);
        box.right  = (std::max)(box.right, position[0]);
        box.bottom = (std::max)(box.bottom, position[1]);
    }

    return box;
}

auto windowBounds(const uintptr_t window) -> ScreenBox
{
    std::array<int16_t, 4> rect{};
    raw::read(window + offsetof(raw::CTkMenuCtrlData, m_WinRect), rect);
    return { rect[0] - kFarMargin, rect[1] - kFarMargin, rect[2] + kFarMargin, rect[3] + kFarMargin };
}

auto outside(const ScreenBox& box, const ScreenBox& bounds) -> bool
{
    return box.valid() && !box.meets(bounds);
}

} // namespace menumanager
