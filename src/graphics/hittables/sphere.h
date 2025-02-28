#pragma once

#include "hittable.h"

#include "graphics/interval.h"
#include "graphics/materials/rt_material.h"
#include "graphics/ray.h"

#include "glm/gtx/norm.hpp"

class Sphere : public Hittable {
public:
    Sphere(const glm::dvec3& static_center, double radius, RTMaterial* mat) :
        center(static_center, glm::dvec3(0.0)), radius(std::fmax(0, radius)), mat(mat)
    {}

    // Moving Sphere
    Sphere(const glm::dvec3& center1, const glm::dvec3& center2, double radius, RTMaterial* mat) :
        center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat)
    {}


    ~Sphere() { delete mat; }

    bool hit(const Ray& ray, const Interval& interval, hit_record& rec) const override;

private:
    Ray center;
    double radius;
    RTMaterial* mat = nullptr;
};
