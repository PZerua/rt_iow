#pragma once

#include "graphics/hittables/hittable.h"

class RtMaterial {

public:
    virtual ~RtMaterial() = default;

    virtual glm::dvec3 emitted(double u, double v, const glm::dvec3& p) const {
        return glm::dvec3(0, 0, 0);
    }

    virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const {
        return false;
    }

};
