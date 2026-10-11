#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// A loaded art file. Laid out differently from the PS2 version
struct YmMenuShapeFile
{
    uint32_t shapes; // array of shape pointers, shape_num long
    char     id[16];
    uint8_t  pad14[8];
    uint16_t shape_num;
};

static_assert(offsetof(YmMenuShapeFile, id) == 0x04 && offsetof(YmMenuShapeFile, shape_num) == 0x1C);

#pragma pack(pop)

} // namespace menumanager::raw
