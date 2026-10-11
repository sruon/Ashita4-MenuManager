#include "shape.h"

#include <menumanager/raw/br_source_rec.h>
#include <menumanager/raw/fr_source_rec.h>
#include <menumanager/raw/memory.h>
#include <menumanager/raw/tk_draw_ctrl_button.h>
#include <menumanager/raw/tk_draw_ctrl_frame.h>
#include <menumanager/raw/tk_menu_frame.h>
#include <menumanager/raw/tk_resource_id.h>
#include <menumanager/raw/ym_menu_shape_file.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <limits>

namespace menumanager
{

using namespace raw;

auto readShape(FFXI::menuobject_t& object) -> bool
{
    const bool button     = object.Type == FFXI::MenuObjectType::Button;
    const auto shapeAt    = object.Address + (button ? offsetof(CTkDrawCtrlButton, m_pShape) : offsetof(CTkDrawCtrlFrame, m_pShape));
    const auto resourceAt = object.Address + (button ? offsetof(CTkDrawCtrlButton, m_pMB) : offsetof(CTkDrawCtrlFrame, m_pMF));
    const auto overrideAt = object.Address + offsetof(CTkDrawCtrlButton, m_pUserShape);

    uint32_t shape = 0, overrideShape = 0, resource = 0;
    if (!read(shapeAt, shape) || !read(resourceAt, resource) || (button && !read(overrideAt, overrideShape)))
    {
        return false;
    }

    object.Shape      = shape;
    object.Overridden = overrideShape != 0;
    if (shape != 0 && shape < kPointerFloor)
    {
        return false;
    }

    if (shape == 0 || object.Overridden || resource == 0)
    {
        return true;
    }

    // look for the shape among the art the frame or button declares
    CTkMenuFrame resources{};
    uint8_t      count = 0;
    if (!read(resource, resources) || !read(resources.m_pRR + (button ? offsetof(BRSOURCEREC, resourceCount) : offsetof(FRSOURCEREC, resourceCount)), count))
    {
        return false;
    }

    bool ambiguous = false;
    for (uint32_t i = 0; i < count; ++i)
    {
        CTkResourceID entry{};
        if (!read(resources.m_pResources + i * sizeof(CTkResourceID), entry))
        {
            return false;
        }

        if (entry.m_Shapes == 0)
        {
            continue; // art that isn't in use may not be loaded
        }

        YmMenuShapeFile file{};
        uint32_t        candidate = 0;
        const auto      index     = entry.m_RscNum;
        if (!read(entry.m_Shapes, file) || index < 0 || index >= file.shape_num || file.shapes < kPointerFloor ||
            uint64_t{ file.shapes } + static_cast<uint32_t>(index) * sizeof(uint32_t) > std::numeric_limits<uint32_t>::max() - sizeof(uint32_t) || !read(file.shapes + static_cast<uint32_t>(index) * sizeof(uint32_t), candidate))
        {
            return false;
        }

        if (candidate != shape)
        {
            continue;
        }

        if (!std::ranges::equal(file.id, entry.m_RscID))
        {
            return false;
        }

        // the same shape found under a second file or index can't be attributed to either
        ambiguous |= object.Matched && (!std::ranges::equal(object.Library, entry.m_RscID) || object.Index != index);
        std::ranges::copy(entry.m_RscID, object.Library);
        object.Index   = index;
        object.Matched = 1;
    }

    object.Matched = object.Matched && !ambiguous;

    uint32_t shapeAfter = 0, resourceAfter = 0, overrideAfter = 0;
    return read(shapeAt, shapeAfter) && shapeAfter == shape && read(resourceAt, resourceAfter) && resourceAfter == resource &&
           (!button || (read(overrideAt, overrideAfter) && overrideAfter == overrideShape));
}

} // namespace menumanager
