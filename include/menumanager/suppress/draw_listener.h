#pragma once

#include <menumanager/suppress/draw_call.h>

#include <cstdint>

namespace menumanager
{

// Decides which windows a WindowSuppressor hides and sees what they draw. Every call comes from the game's thread,
// inside CTkMenuCtrlData::OnDraw. A window can draw inside another one's draw, and the calls nest the same way
class DrawListener
{
public:
    virtual ~DrawListener() = default;

    // Before the window draws. Returning true hides it
    virtual auto beginWindow(uintptr_t window) -> bool = 0;
    // After it drew. hidden is what beginWindow returned
    virtual auto endWindow(uintptr_t window, bool hidden) -> void;
    // Each draw the window makes, hidden or not, before it is dropped or kept
    virtual auto draw(uintptr_t window, const DrawCall& call) -> void;
};

} // namespace menumanager
