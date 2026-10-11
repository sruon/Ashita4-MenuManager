#pragma once

#include <menumanager/ffxi/menu_key.h>

#include <cstdint>
#include <vector>

namespace menumanager
{

struct IMenuManager;

// One frame of input for a slider
struct SliderStep
{
    int32_t                    button{};     // the slider. Set even on the final Ok, after the driver has gone idle
    bool                       moveCursor{}; // move the window's cursor to the slider before the keys
    std::vector<FFXI::MenuKey> keys;

    // Sends the step to the window, checking CanPress before each input since a key can move focus or close it.
    // Returns false if it stopped early
    auto apply(IMenuManager& menus, uintptr_t window) const -> bool;
};

// Moves a config slider to a value the way the keyboard does. Ok on the slider starts adjusting it, Left and Right
// step it, and Ok again keeps the value. Ranges differ per slider (100 steps for volumes, 127 for others), so the
// driver steers by the fill read back each frame instead of counting steps.
// Which key raises a slider isn't proven on PC. The PS2 FsGameConfigMenu raises on Right, and the driver learns the
// direction from the first step that moves the bar
class SliderDriver
{
public:
    // target is 0 to 1
    auto retarget(int32_t button, float target) -> void;
    // fill is the slider's value as drawn, 0 to 1, or negative once the window no longer shows it. That ends the drive
    auto step(float fill) -> SliderStep;
    // The slider being driven, 0 when idle
    auto button() const -> int32_t;

private:
    int32_t       button_{};
    float         target_{};
    bool          adjusting_{};     // Ok was sent, so Left and Right change the value
    bool          commitPending_{}; // retargeted while adjusting another slider, which needs an Ok first
    FFXI::MenuKey raise_ = FFXI::MenuKey::Right;
    float         last_  = -1; // the fill when the last keys went out
    int32_t       sent_{};     // how many steps went out then, negative when lowering
    bool          overshot_{}; // passed the target once, so single steps from here
    int32_t       stuck_{};    // frames the bar didn't move after steps
    bool          moving_{};   // a step has moved the bar, so the raise key is known
    bool          swapped_{};  // already tried the keys the other way round
    int32_t       frames_{};
};

} // namespace menumanager
