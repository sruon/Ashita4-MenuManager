#pragma once

namespace menumanager
{

// A rectangle in the pixels the game's pre-transformed vertices use
struct ScreenBox
{
    float left;
    float top;
    float right;
    float bottom;

    // Holds nothing. Not valid until a point is added
    static auto none() -> ScreenBox;
    auto        valid() const -> bool;
    // Whether the two share a point. Touching edges count
    auto meets(const ScreenBox& other) const -> bool;
};

} // namespace menumanager
