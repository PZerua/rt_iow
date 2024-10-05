#pragma once

#include "hittable.h"

#include "graphics/interval.h"

#include "glm/gtx/norm.hpp"

class Sphere : public Hittable {
public:
    Sphere(const glm::dvec3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const Ray& ray, const Interval& interval, hit_record& rec) const override;

private:
    glm::dvec3 center;
    double radius;
};
