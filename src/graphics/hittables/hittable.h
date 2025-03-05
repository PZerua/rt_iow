#pragma once

#include "graphics/ray.h"
#include "graphics/interval.h"
#include "graphics/aabb.h"

#include "framework/math/math_utils.h"

class RtMaterial;

struct hit_record {
    glm::dvec3 p;
    glm::dvec3 normal;
    RtMaterial* mat;
    double t;
    double u;
    double v;
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

    virtual bool hit(const Ray& r, const Interval& interval, hit_record& rec) const = 0;

    virtual RtAABB bounding_box() const = 0;
};
