#include <menumanager/raw/memory.h>

#include <windows.h>

#include <cstring>

namespace menumanager::raw
{

// Keep local objects with destructors out of this function: MSVC rejects them alongside __try (C2712)
auto readBytes(void* destination, const uintptr_t source, const size_t count) -> bool
{
    if (source < kPointerFloor)
    {
        return false;
    }

    __try
    {
        std::memcpy(destination, reinterpret_cast<const void*>(source), count);
        return true;
    }

    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

} // namespace menumanager::raw
