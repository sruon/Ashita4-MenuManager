#include <menumanager/suppress/draw_listener.h>

#include <menumanager/suppress/draw_call.h>

#include <cstdint>

namespace menumanager
{

auto DrawListener::endWindow(uintptr_t, bool) -> void
{
}

auto DrawListener::draw(uintptr_t, const DrawCall&) -> void
{
}

} // namespace menumanager
