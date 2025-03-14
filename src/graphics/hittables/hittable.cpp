#include "hittable.h"

AABB Hittable::global_bounding_box() const
{
    return bbox.transform(transform.get_model());
}
