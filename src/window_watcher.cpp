#include <menumanager/window_watcher.h>

#include <menumanager/menu_manager.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <utility>
#include <vector>

namespace menumanager
{

auto WindowWatcher::update(const IMenuManager& menus) -> void
{
    std::vector<uintptr_t> windows(menus.GetWindows(nullptr, 0));
    windows.resize(std::min<size_t>(windows.size(), menus.GetWindows(windows.data(), static_cast<uint32_t>(windows.size()))));
    std::ranges::sort(windows);
    windows.erase(std::ranges::unique(windows).begin(), windows.end());

    opened_.clear();
    closed_.clear();
    std::ranges::set_difference(windows, windows_, std::back_inserter(opened_));
    std::ranges::set_difference(windows_, windows, std::back_inserter(closed_));
    windows_ = std::move(windows);
}

auto WindowWatcher::opened() const -> std::span<const uintptr_t>
{
    return opened_;
}

auto WindowWatcher::closed() const -> std::span<const uintptr_t>
{
    return closed_;
}

} // namespace menumanager
