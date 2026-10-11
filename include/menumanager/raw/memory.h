#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace menumanager::raw
{

// Anything below this is null or a small number, not a pointer
constexpr uintptr_t kPointerFloor = 0x10000;

// Copies game memory. Returns false instead of crashing on a bad address
auto readBytes(void* destination, uintptr_t source, size_t count) -> bool;

template <typename T>
    requires std::is_trivially_copyable_v<T>
auto read(const uintptr_t address, T& out) -> bool
{
    return readBytes(&out, address, sizeof(T));
}

} // namespace menumanager::raw
