#pragma once

#include "hittable.h"

#include <vector>

class HittableList : public Hittable
{
public:
    std::vector<Hittable*> objects;

    HittableList() {}
    HittableList(Hittable* object) { add(object); }

    void clear() { objects.clear(); }

    void add(Hittable* object) {
        objects.push_back(object);
        bbox = merge_aabbs(bbox, object->bounding_box());
    }

    bool hit(const Ray& r, const Interval& interval, hit_record& rec) const override;

private:
};
