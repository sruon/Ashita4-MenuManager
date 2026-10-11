#pragma once

#include <menumanager/suppress/screen_box.h>

#include <d3d8.h>

#include <cstdint>

namespace menumanager
{

// The Direct3D call a draw came through
enum class DrawSource : uint8_t
{
    Buffer,             // DrawPrimitive or DrawIndexedPrimitive
    UserPointer,        // DrawPrimitiveUP
    IndexedUserPointer, // DrawIndexedPrimitiveUP
};

// One Direct3D draw made while a window draws
struct DrawCall
{
    IDirect3DDevice8* device;
    DrawSource        source;
    D3DPRIMITIVETYPE  type;
    uint32_t          count;    // primitives
    const void*       vertices; // null for Buffer
    uint32_t          stride;
    ScreenBox         box; // where the vertices land when they are pre-transformed, otherwise ScreenBox::none()
};

} // namespace menumanager
