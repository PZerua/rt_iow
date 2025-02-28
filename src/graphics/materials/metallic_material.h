#pragma once

#include "rt_material.h"

#include "framework/math/math_utils.h"

class MetallicMaterial : public RTMaterial {

    glm::dvec3 albedo;
    double fuzz;

public:
    MetallicMaterial(const glm::dvec3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const override
    {
        glm::dvec3 reflected = glm::reflect(r_in.direction(), rec.normal);
        reflected = glm::normalize(reflected) + (fuzz * random_unit_sphere_direction()); // apply random offset to reflection to have fuzziness

        scattered = Ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        return true;
    }

};
