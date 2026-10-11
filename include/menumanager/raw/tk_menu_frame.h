#pragma once

#include <cstddef>
#include <cstdint>

namespace menumanager::raw
{

#pragma pack(push, 1)

// The resources a frame or button can draw: its source record (FRSOURCEREC or BRSOURCEREC) and an array of
// CTkResourceID, as long as the record's resourceCount. Frames and buttons share this layout
struct CTkMenuFrame
{
    uint32_t m_pRR;
    uint32_t m_pResources;
};

using CTkMenuButton = CTkMenuFrame;

#pragma pack(pop)

} // namespace menumanager::raw
