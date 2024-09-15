#pragma once

#include "hittable.h"

#include <vector>

class Hittable_list : public Hittable
{
public:
    std::vector<Hittable*> objects;

    Hittable_list() {}
    Hittable_list(Hittable* object) { add(object); }

    void clear() { objects.clear(); }

    void add(Hittable* object) {
        objects.push_back(object);
    }

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override;
};
