#pragma once

#include "graphics/aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "graphics/utils.h"

class BVHNode : public Hittable {
public:
    BVHNode(HittableList list);

    BVHNode(std::vector<Hittable*>& objects, size_t start, size_t end);

    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;

private:
    Hittable* left = nullptr;
    Hittable* right = nullptr;

    static bool box_compare(
        const Hittable* a, const Hittable* b, int axis_index
    ) {
        Interval a_axis_interval = axis_interval(a->global_bounding_box(), axis_index);
        Interval b_axis_interval = axis_interval(b->global_bounding_box(), axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const Hittable* a, const Hittable* b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const Hittable* a, const Hittable* b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const Hittable* a, const Hittable* b) {
        return box_compare(a, b, 2);
    }
};
