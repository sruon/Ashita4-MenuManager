#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A button's source record, as loaded from the DAT
struct BRSOURCEREC
{
    uint8_t pad00[0x1B];
    uint8_t resourceCount;
};

static_assert(offsetof(BRSOURCEREC, resourceCount) == 0x1B);

#pragma pack(pop)

} // namespace menumanager::raw
