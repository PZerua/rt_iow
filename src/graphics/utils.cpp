#include "utils.h"

#include "interval.h"
#include "ray.h"

#include <cmath>

double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

Interval axis_interval(const AABB& aabb, int axis)
{
    glm::dvec3 aabb_corner0 = (aabb.center - aabb.half_size);
    glm::dvec3 aabb_corner1 = (aabb.center + aabb.half_size);

    glm::dvec3 interval_min = glm::min(aabb_corner0, aabb_corner1);
    glm::dvec3 interval_max = glm::max(aabb_corner0, aabb_corner1);

    if (axis == 1) return { interval_min.y, interval_max.y };
    if (axis == 2) return { interval_min.z, interval_max.z };
    return { interval_min.x, interval_max.x };
}

bool ray_aabb_interval_intersection(const Ray& ray, Interval ray_t, const AABB& aabb)
{
    const glm::dvec3& ray_orig = ray.origin();
    const glm::dvec3& ray_dir = ray.direction();

    for (int axis = 0; axis < 3; axis++) {
        const Interval& ax = axis_interval(aabb, axis);
        const double adinv = 1.0 / ray_dir[axis];

        auto t0 = (ax.min - ray_orig[axis]) * adinv;
        auto t1 = (ax.max - ray_orig[axis]) * adinv;

        if (t0 < t1) {
            if (t0 > ray_t.min) ray_t.min = t0;
            if (t1 < ray_t.max) ray_t.max = t1;
        }
        else {
            if (t1 > ray_t.min) ray_t.min = t1;
            if (t0 < ray_t.max) ray_t.max = t0;
        }

        if (ray_t.max <= ray_t.min)
            return false;
    }
    return true;
}
