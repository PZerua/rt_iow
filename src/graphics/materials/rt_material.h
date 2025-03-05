#pragma once

#include "graphics/hittables/hittable.h"

class RtMaterial {

public:
    virtual ~RtMaterial() = default;

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const {
        return false;
    }
};
