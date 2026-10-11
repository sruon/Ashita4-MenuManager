#include <menumanager/slider_driver.h>

#include <menumanager/menu_manager.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace menumanager
{

namespace
{

constexpr float   kDoneWithin = 0.005f; // a step is 1/100 or 1/127, so closer than this is done
constexpr int32_t kMaxFrames  = 300;
constexpr int32_t kStuckAfter = 3;  // frames without movement before the bar counts as stuck at an end
constexpr float   kStepsPer   = 50; // steps sent per unit of distance on the first pass
constexpr int32_t kMaxSteps   = 25;

constexpr auto opposite(const FFXI::MenuKey key) -> FFXI::MenuKey
{
    return key == FFXI::MenuKey::Right ? FFXI::MenuKey::Left : FFXI::MenuKey::Right;
}

} // namespace

auto SliderStep::apply(IMenuManager& menus, const uintptr_t window) const -> bool
{
    if (moveCursor)
    {
        if (!menus.CanPress(window, button))
        {
            return false;
        }

        menus.SetWindowCursor(window, button);
    }

    for (const auto key : keys)
    {
        if (!menus.CanPress(window, button))
        {
            return false;
        }

        menus.PressKey(key);
    }

    return true;
}

auto SliderDriver::retarget(const int32_t button, const float target) -> void
{
    if (button_ == button)
    {
        target_   = target;
        overshot_ = false;
        frames_   = 0;
        return;
    }

    const bool wasAdjusting = button_ != 0 && adjusting_;
    *this                   = {};
    button_                 = button;
    target_                 = target;
    commitPending_          = wasAdjusting;
}

auto SliderDriver::step(const float fill) -> SliderStep
{
    SliderStep step{ .button = button_ };
    if (button_ == 0)
    {
        return step;
    }

    if (fill < 0)
    {
        *this = {};
        return step;
    }

    if (commitPending_)
    {
        commitPending_ = false;
        step.keys      = { FFXI::MenuKey::Ok };
        return step;
    }

    if (!adjusting_)
    {
        adjusting_      = true;
        step.moveCursor = true;
        step.keys       = { FFXI::MenuKey::Ok };
        return step;
    }

    const float diff   = target_ - fill;
    bool        finish = std::abs(diff) <= kDoneWithin || ++frames_ > kMaxFrames;
    if (sent_ != 0 && !finish)
    {
        const float moved = fill - last_;
        if (moved == 0 && !moving_ && !swapped_)
        {
            // pushed against the end it was already at, or the keys are the other way round
            raise_   = opposite(raise_);
            swapped_ = true;
        }
        else if (moved == 0)
        {
            finish = ++stuck_ >= kStuckAfter;
        }
        else
        {
            stuck_  = 0;
            moving_ = true;
            if ((moved > 0) != (sent_ > 0))
            {
                // the bar went the other way
                raise_ = opposite(raise_);
            }
            else if ((diff > 0) != (sent_ > 0))
            {
                // passed the target. Go back in single steps, and a second pass means it's within a step
                finish    = overshot_;
                overshot_ = true;
            }
        }
    }

    if (finish)
    {
        *this     = {};
        step.keys = { FFXI::MenuKey::Ok };
        return step;
    }

    const int32_t count = overshot_ ? 1 : std::clamp(static_cast<int32_t>(std::abs(diff) * kStepsPer), 1, kMaxSteps);
    step.keys.assign(static_cast<size_t>(count), diff > 0 ? raise_ : opposite(raise_));
    last_ = fill;
    sent_ = diff > 0 ? count : -count;
    return step;
}

auto SliderDriver::button() const -> int32_t
{
    return button_;
}

} // namespace menumanager
