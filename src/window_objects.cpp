#include "menu_manager.h"
#include "shape.h"

#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_draw_ctrl_button.h>
#include <menumanager/raw/tk_draw_ctrl_cursol.h>
#include <menumanager/raw/tk_draw_ctrl_frame.h>
#include <menumanager/raw/tk_menu_ctrl_data.h>
#include <menumanager/raw/tk_ob_list.h>

#include <cstddef>

namespace menumanager
{

using namespace raw;

namespace
{

// A button's number, status, position and size
auto readButton(const uint32_t object, FFXI::menuobject_t& entry) -> bool
{
    return readBytes(&entry.X, object + offsetof(CTkDrawCtrlButton, m_LocX), 4 * sizeof(int16_t)) && read(object + offsetof(CTkDrawCtrlButton, m_ButtonNo), entry.Number) &&
           read(object + offsetof(CTkDrawCtrlButton, m_MenuCtrlID), entry.Status);
}

auto readCursor(const uint32_t object, FFXI::menuobject_t& entry) -> bool
{
    return readBytes(&entry.X, object + offsetof(CTkDrawCtrlCursol, m_LocX), 2 * sizeof(int16_t));
}

} // namespace

auto MenuManager::GetWindowObjects(const uintptr_t window, FFXI::menuobject_t* objects, const uint32_t capacity, const bool shapes, bool* complete) const -> uint32_t
{
    uint32_t node = 0, head = 0, count = 0;
    bool     intact = false;
    if (window >= kPointerFloor && read(window + offsetof(CTkMenuCtrlData, m_DrawShapes), node))
    {
        head   = node;
        intact = true;
        for (int32_t step = 0; node >= kPointerFloor && step < kMaxObjects; ++step)
        {
            uint32_t object  = 0;
            uint8_t  removed = 1;
            if (!read(node + offsetof(CTkNode, m_pOb), object))
            {
                intact = false;
                break;
            }

            intact = read(node + offsetof(CTkNode, m_IsFree), removed) && intact;
            if (!removed)
            {
                FFXI::menuobject_t entry{ .Address = object, .Index = -1 };
                const bool         typed = read(object + offsetof(CTkDrawCtrlFrame, m_RecKind), entry.Type);
                intact                   = typed && intact;
                // frames and buttons are always listed, the cursor only if its position could be read
                bool listed = false;
                if (typed && entry.Type == FFXI::MenuObjectType::Button)
                {
                    intact = readButton(object, entry) && intact;
                }
                else if (typed && entry.Type == FFXI::MenuObjectType::Cursor)
                {
                    listed = readCursor(object, entry);
                }

                if (typed && (entry.Type == FFXI::MenuObjectType::Frame || entry.Type == FFXI::MenuObjectType::Button))
                {
                    if (shapes)
                    {
                        intact = readShape(entry) && intact;
                    }

                    listed = true;
                }

                if (listed)
                {
                    if (count < capacity && objects != nullptr)
                    {
                        objects[count] = entry;
                    }

                    ++count;
                }
            }

            // keep this object even if the link to the next one can't be read
            if (!read(node + offsetof(CTkNode, m_pNextNode), node))
            {
                intact = false;
                break;
            }
        }

        // complete only if the list ended and its head didn't change during the walk
        uint32_t headAfter = 0;
        intact             = node == 0 && read(window + offsetof(CTkMenuCtrlData, m_DrawShapes), headAfter) && headAfter == head && intact;
    }

    if (complete != nullptr)
    {
        *complete = intact;
    }

    return count;
}

} // namespace menumanager
