#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A frame's source record, as loaded from the DAT
struct FRSOURCEREC
{
    uint8_t pad00[0x14];
    uint8_t resourceCount;
};

static_assert(offsetof(FRSOURCEREC, resourceCount) == 0x14);

#pragma pack(pop)

} // namespace menumanager::raw
