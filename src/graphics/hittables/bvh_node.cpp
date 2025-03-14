#include "bvh_node.h"

#include <algorithm>

BVHNode::BVHNode(HittableList list) : BVHNode(list.objects, 0, list.objects.size())
{
}

BVHNode::BVHNode(std::vector<Hittable*>& objects, size_t start, size_t end)
{
    // Build the bounding box of the span of source objects.
    bbox = AABB();
    for (size_t object_index = start; object_index < end; object_index++)
        bbox = merge_aabbs(bbox, objects[object_index]->bounding_box());

    int axis = bbox.longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    }
    else if (object_span == 2) {
        left = objects[start];
        right = objects[start + 1];
    }
    else {
        std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

        size_t mid = start + object_span / 2;
        left = new BVHNode(objects, start, mid);
        right = new BVHNode(objects, mid, end);
    }
}

inline bool BVHNode::hit(const Ray& r, const Interval& ray_t, hit_record& rec) const {
    if (!bbox.ray_intersection(r.origin(), r.direction(), ray_t.min, ray_t.max))
        return false;

    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    return hit_left || hit_right;
}
