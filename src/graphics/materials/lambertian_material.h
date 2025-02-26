#pragma once

#include "rt_material.h"

#include "framework/math/math_utils.h"

class LambertianMaterial : public RTMaterial {

    glm::dvec3 albedo;

public:
    LambertianMaterial(const glm::dvec3& albedo) : albedo(albedo) {}

    bool scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_sphere_direction();

        // Catch degenerate scatter direction
        if (is_direction_near_zero(scatter_direction)) {
            scatter_direction = rec.normal;
        }

        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

};
