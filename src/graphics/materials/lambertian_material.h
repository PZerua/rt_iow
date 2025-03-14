#pragma once

#include "rt_material.h"

#include "graphics/rt_texture.h"

#include "framework/math/math_utils.h"

class LambertianMaterial : public RtMaterial {

    RtTexture* tex = nullptr;

public:
    LambertianMaterial(const glm::dvec3& albedo) : tex(new SolidColor(albedo)) {}
    LambertianMaterial(RtTexture* tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_sphere_direction();

        // Catch degenerate scatter direction
        if (is_direction_near_zero(scatter_direction)) {
            scatter_direction = rec.normal;
        }

        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

};
