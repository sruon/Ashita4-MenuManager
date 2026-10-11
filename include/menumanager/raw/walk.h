#pragma once

#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_ob_list.h>

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

// Calls visit(object) for each live node of a CTkObList, starting at `node`, until visit returns false.
// Stops after `limit` nodes or at the first unreadable link
auto forEachNode(uint32_t node, const int32_t limit, std::predicate<uint32_t> auto&& visit) -> void
{
    for (int32_t step = 0; node >= kPointerFloor && step < limit; ++step)
    {
        uint32_t object  = 0;
        uint8_t  removed = 1;
        if (!read(node + offsetof(CTkNode, m_pOb), object))
        {
            return;
        }

        read(node + offsetof(CTkNode, m_IsFree), removed);
        if ((!removed && !visit(object)) || !read(node + offsetof(CTkNode, m_pNextNode), node))
        {
            return;
        }
    }
}

} // namespace menumanager::raw
