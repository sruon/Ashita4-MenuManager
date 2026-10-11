#include "draw_box.h"

#include <menumanager/suppress/draw_call.h>
#include <menumanager/suppress/draw_listener.h>
#include <menumanager/suppress/screen_box.h>
#include <menumanager/suppress/window_suppressor.h>

#include <d3d8.h>

#include <cstdint>

namespace menumanager
{

auto WindowSuppressor::setListener(DrawListener* listener) -> void
{
    listener_ = listener;
}

auto WindowSuppressor::setKeepOutsideDraws(const bool keep) -> void
{
    keepOutside_ = keep;
}

auto WindowSuppressor::drawWindow(const uintptr_t window, const WindowDrawFn draw, void* edx) -> void
{
    auto* const listener = listener_;
    if (listener == nullptr || depth_ == kMaxDepth)
    {
        draw(window, edx);
        return;
    }

    auto& frame = frames_[depth_];
    frame       = { .window = window, .listener = listener, .hidden = listener->beginWindow(window), .bounds = windowBounds(window) };
    ++depth_;
    draw(window, edx);
    --depth_;
    listener->endWindow(window, frame.hidden);
}

auto WindowSuppressor::current() const -> const Frame*
{
    return depth_ == 0 ? nullptr : &frames_[depth_ - 1];
}

auto WindowSuppressor::onDrawPrimitive(IDirect3DDevice8* device, const D3DPRIMITIVETYPE type, const uint32_t count) const -> bool
{
    const auto* frame = current();
    if (frame == nullptr)
    {
        return false;
    }

    frame->listener->draw(frame->window, { .device = device, .source = DrawSource::Buffer, .type = type, .count = count, .vertices = nullptr, .stride = 0, .box = ScreenBox::none() });
    return frame->hidden;
}

auto WindowSuppressor::onDrawPrimitiveUP(IDirect3DDevice8* device, const D3DPRIMITIVETYPE type, const uint32_t count, const void* vertices, const uint32_t stride) const -> bool
{
    const auto* frame = current();
    if (frame == nullptr)
    {
        return false;
    }

    const DrawCall call{ .device = device, .source = DrawSource::UserPointer, .type = type, .count = count, .vertices = vertices, .stride = stride, .box = drawBox(vertexShader(device), vertices, 0, vertexCount(type, count), stride) };
    frame->listener->draw(frame->window, call);
    return frame->hidden && !(keepOutside_ && outside(call.box, frame->bounds));
}

auto WindowSuppressor::onDrawIndexedPrimitiveUP(IDirect3DDevice8* device, const D3DPRIMITIVETYPE type, const uint32_t minIndex, const uint32_t vertexCount, const uint32_t count, const void* vertices, const uint32_t stride) const -> bool
{
    const auto* frame = current();
    if (frame == nullptr)
    {
        return false;
    }

    const DrawCall call{ .device = device, .source = DrawSource::IndexedUserPointer, .type = type, .count = count, .vertices = vertices, .stride = stride, .box = drawBox(vertexShader(device), vertices, minIndex, vertexCount, stride) };
    frame->listener->draw(frame->window, call);
    return frame->hidden;
}

} // namespace menumanager
