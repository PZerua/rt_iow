#pragma once

#include "rt_material.h"

#include "framework/math/math_utils.h"

class DielectricMaterial : public RtMaterial {

    double refraction_index;

public:
    DielectricMaterial(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const override
    {
        attenuation = glm::dvec3(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        glm::dvec3 unit_direction = glm::normalize(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        glm::dvec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_d())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = Ray(rec.p, direction, r_in.time());
        return true;
    }

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};
