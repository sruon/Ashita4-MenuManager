#include "menu_manager.h"

#include <menumanager/raw/tk_menu_mng.h>
#include <menumanager/raw/walk.h>

#include <cstddef>
#include <cstdint>

namespace menumanager
{

using namespace raw;

auto MenuManager::GetWindows(uintptr_t* windows, const uint32_t capacity) const -> uint32_t
{
    uint32_t head = 0, count = 0;
    if (read(manager_ + offsetof(CTkMenuMng, m_DrawDatas), head))
    {
        forEachNode(head, kMaxWindows, [&](const uint32_t window)
                    {
                        if (windows != nullptr && count < capacity)
                        {
                            windows[count] = window;
                        }

                        ++count;
                        return true;
                    });
    }

    return count;
}

} // namespace menumanager
