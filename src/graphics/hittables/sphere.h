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
    {
        glm::dvec3 rvec = glm::dvec3(radius, radius, radius);
        bbox = RtAABB(static_center - rvec, static_center + rvec);
    }

    // Moving Sphere
    Sphere(const glm::dvec3& center1, const glm::dvec3& center2, double radius, RTMaterial* mat) :
        center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat)
    {
        glm::dvec3 rvec = glm::dvec3(radius, radius, radius);
        RtAABB box1(center.at(0) - rvec, center.at(0) + rvec);
        RtAABB box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = RtAABB(box1, box2);
    }

    ~Sphere() { delete mat; }

    bool hit(const Ray& ray, const Interval& interval, hit_record& rec) const override;

    RtAABB bounding_box() const override { return bbox; }

private:
    Ray center;
    double radius;
    RTMaterial* mat = nullptr;
    RtAABB bbox;
};
