#include <menumanager/suppress/window_suppressor.h>

#include <Ashita.h>
#include <MinHook.h>

#include <cstdint>
#include <cstring>
#include <memory>

namespace menumanager
{

namespace
{

WindowSuppressor* active = nullptr;
// The trampoline. A pinned module that loads again finds its hook still in place, and this still holds it
WindowDrawFn original = nullptr;

void __fastcall detour(const uintptr_t window, void* edx)
{
    if (active == nullptr)
    {
        original(window, edx);
        return;
    }

    active->drawWindow(window, original, edx);
}

// MinHook's state is shared with anything else in the module that uses it
auto minHookReady() -> bool
{
    static const bool ready = []
    {
        const auto status = MH_Initialize();
        return status == MH_OK || status == MH_ERROR_ALREADY_INITIALIZED;
    }();
    return ready;
}

} // namespace

WindowSuppressor::WindowSuppressor(const uintptr_t onDraw)
{
    if (onDraw == 0 || active != nullptr || !minHookReady())
    {
        return;
    }

    auto* const at      = reinterpret_cast<void*>(onDraw);
    const auto  created = MH_CreateHook(at, reinterpret_cast<void*>(&detour), reinterpret_cast<void**>(&original));
    if (created != MH_OK && created != MH_ERROR_ALREADY_CREATED)
    {
        return;
    }

    if (const auto enabled = MH_EnableHook(at); enabled != MH_OK && enabled != MH_ERROR_ENABLED)
    {
        MH_RemoveHook(at);
        return;
    }

    target_ = static_cast<uint8_t*>(at);
    std::memcpy(patch_.data(), target_, patch_.size());
    active = this;
}

WindowSuppressor::~WindowSuppressor()
{
    unhook();
}

auto WindowSuppressor::hooked() const -> bool
{
    return target_ != nullptr;
}

auto WindowSuppressor::unhook() -> bool
{
    if (active == this)
    {
        active = nullptr;
    }

    if (target_ == nullptr)
    {
        return true;
    }

    // MinHook writes its saved bytes back without checking, so a prologue someone else patched since is left alone
    if (std::memcmp(target_, patch_.data(), patch_.size()) != 0 || MH_RemoveHook(target_) != MH_OK)
    {
        return false;
    }

    target_ = nullptr;
    return true;
}

auto attachWindowSuppressor(const uintptr_t onDraw) -> std::unique_ptr<WindowSuppressor>
{
    auto suppressor = std::make_unique<WindowSuppressor>(onDraw);
    if (!suppressor->hooked())
    {
        return nullptr;
    }

    return suppressor;
}

auto createWindowSuppressor(const uintptr_t moduleBase, const uintptr_t moduleSize, const char** missing) -> std::unique_ptr<WindowSuppressor>
{
    const auto report = [&](const char* what)
    {
        if (missing != nullptr)
        {
            *missing = what;
        }
    };
    const auto onDraw = Ashita::Memory::FindPattern(moduleBase, moduleSize, kWindowDrawPattern, 0, 0);
    if (onDraw == 0)
    {
        report("window draw");
        return nullptr;
    }

    auto suppressor = attachWindowSuppressor(onDraw);
    report(suppressor == nullptr ? "window draw hook" : nullptr);
    return suppressor;
}

} // namespace menumanager
