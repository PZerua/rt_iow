#pragma once

#include "graphics/aabb.h"
#include "hittable.h"
#include "hittable_list.h"

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
        float a_axis_interval = a->bounding_box().axis_min(axis_index);
        float b_axis_interval = b->bounding_box().axis_min(axis_index);
        return a_axis_interval < b_axis_interval;
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
