#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace menumanager
{

struct IMenuManager;

// Tells which windows opened and closed between two calls to update. Call it once a frame
class WindowWatcher
{
public:
    auto update(const IMenuManager& menus) -> void;
    // Windows open now that weren't at the last update, ordered by address
    auto opened() const -> std::span<const uintptr_t>;
    // Windows open at the last update that aren't now, ordered by address
    auto closed() const -> std::span<const uintptr_t>;

private:
    std::vector<uintptr_t> windows_; // sorted
    std::vector<uintptr_t> opened_;
    std::vector<uintptr_t> closed_;
};

} // namespace menumanager
