#include "hittable_list.h"

bool Hittable_list::hit(const Ray& r, const Interval& interval, hit_record& rec) const
{
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = interval.max;

    for (const auto& object : objects) {
        if (object->hit(r, { interval.min, closest_so_far }, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}
