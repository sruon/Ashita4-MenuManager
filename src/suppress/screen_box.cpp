#include <menumanager/suppress/screen_box.h>

namespace menumanager
{

namespace
{

constexpr float kFar = 1e9f;

} // namespace

auto ScreenBox::none() -> ScreenBox
{
    return { kFar, kFar, -kFar, -kFar };
}

auto ScreenBox::valid() const -> bool
{
    return left <= right;
}

auto ScreenBox::meets(const ScreenBox& other) const -> bool
{
    return right >= other.left && left <= other.right && bottom >= other.top && top <= other.bottom;
}

} // namespace menumanager
