#pragma once

#include <menumanager/ffxi/menu_window.h>

namespace menumanager
{

// Whether the window is in its opening or closing animation. The game then draws only the frame, blurred and scaled,
// and skips the buttons, the text and the cursor (CTkMenuCtrlData::OnDraw)
auto isAnimating(const FFXI::menuwindow_t& window) -> bool;

// How opaque the game draws the window's frame, from 0 to 1, by its fade (CTkDrawCtrlFrame::ShapeCalc).
// A window without a frame is 1
auto opacity(const FFXI::menuwindow_t& window) -> float;

} // namespace menumanager
