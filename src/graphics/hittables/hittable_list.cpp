#include "hittable_list.h"

bool HittableList::hit(const Ray& ray, const Interval& interval, hit_record& rec) const
{
    glm::mat4x4 model = transform.get_model();

    Ray ray_local(inv_model * glm::dvec4(ray.origin(), 1.0), inv_model * glm::dvec4(ray.direction(), 0.0), ray.time());

    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = interval.max;

    for (const auto& object : objects) {
        if (object->hit(ray_local, { interval.min, closest_so_far }, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            temp_rec.p = model * glm::dvec4(temp_rec.p, 1.0);
            temp_rec.normal = model * glm::dvec4(temp_rec.normal, 0.0);
            rec = temp_rec;
        }
    }

    return hit_anything;
}
