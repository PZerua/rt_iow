#pragma once

#include "graphics/ray.h"
#include "graphics/interval.h"

#include "framework/math/transform.h"
#include "framework/math/aabb.h"
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

protected:
    Transform transform;
    AABB bbox;

public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, const Interval& interval, hit_record& rec) const = 0;

    AABB bounding_box() const { return bbox; }
    AABB global_bounding_box() const;

    void set_transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
        transform = Transform(position, rotation, scale);
    }

    void set_position(const glm::vec3& position) {
        transform.set_position(position);
        transform.cache_model();
    }

    void set_rotation(const glm::quat& rotation) {
        transform.set_rotation(rotation);
        transform.cache_model();
    }

    void set_scale(const glm::vec3& scale) {
        transform.set_scale(scale);
        transform.cache_model();
    }
};
