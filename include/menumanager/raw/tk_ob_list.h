#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// CTkObList, the game's linked list, and its nodes
struct CTkNode
{
    uint32_t m_pNextNode;
    uint8_t  pad04[0x0C];
    uint32_t m_pOb;
    uint8_t  m_IsFree; // set once the node is removed
};

static_assert(offsetof(CTkNode, m_pOb) == 0x10 && offsetof(CTkNode, m_IsFree) == 0x14);

struct CTkObList
{
    uint32_t m_pTopNode;
    uint8_t  pad04[0x18];
};

static_assert(sizeof(CTkObList) == 0x1C);

#pragma pack(pop)

} // namespace menumanager::raw
