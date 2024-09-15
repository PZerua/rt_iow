#pragma once

#include "graphics/ray.h"

#include "framework/math/math_utils.h"

struct hit_record {
    glm::dvec3 p;
    glm::dvec3 normal;
    double t;
    bool front_face;

    void set_face_normal(const Ray& r, const glm::dvec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};
