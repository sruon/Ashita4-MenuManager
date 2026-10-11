#pragma once

#include <menumanager/ffxi/menu_object.h>

namespace menumanager
{

// Fills in which art file and index a frame or button is drawing (Shape, Library, Index, Matched, Overridden).
// Returns false if a read failed, or if the object changed while it was being read
auto readShape(FFXI::menuobject_t& object) -> bool;

} // namespace menumanager
