#include <menumanager/window_state.h>

#include <algorithm>

namespace menumanager
{

namespace
{

constexpr float kFadeTicks = 20.0f;

} // namespace

auto isAnimating(const FFXI::menuwindow_t& window) -> bool
{
    return window.Status == FFXI::MenuStatus::Closing || window.OpenTicks > 0;
}

auto opacity(const FFXI::menuwindow_t& window) -> float
{
    if (!window.Framed)
    {
        return 1.0f;
    }

    const float left = static_cast<float>(window.FadeTime) / kFadeTicks;
    switch (window.FadeMode)
    {
        case FFXI::MenuFade::Hidden:
            return 0.0f;
        case FFXI::MenuFade::Out:
            return std::clamp(left, 0.0f, 1.0f);
        case FFXI::MenuFade::In:
            return std::clamp(1.0f - left, 0.0f, 1.0f);
        default:
            return 1.0f;
    }
}

} // namespace menumanager
