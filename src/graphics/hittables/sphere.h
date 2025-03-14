#pragma once

#include "hittable.h"

#include "graphics/interval.h"
#include "graphics/materials/rt_material.h"
#include "graphics/ray.h"

#include "glm/gtx/norm.hpp"

class Sphere : public Hittable {
public:
    Sphere(const glm::dvec3& static_center, double radius, RtMaterial* mat) :
        center(static_center, glm::dvec3(0.0)), radius(std::fmax(0, radius)), mat(mat)
    {
        glm::dvec3 rvec = glm::dvec3(radius, radius, radius);
        bbox = AABB(static_center, rvec);
    }

    // Moving Sphere
    Sphere(const glm::dvec3& center1, const glm::dvec3& center2, double radius, RtMaterial* mat) :
        center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat)
    {
        glm::dvec3 rvec = glm::dvec3(radius, radius, radius);
        AABB box1(center.at(0), rvec);
        AABB box2(center.at(1), rvec);
        bbox = merge_aabbs(box1, box2);
    }

    ~Sphere() { delete mat; }

    bool hit(const Ray& ray, const Interval& interval, hit_record& rec) const override;

    static void get_sphere_uv(const glm::dvec3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = std::acos(-p.y);
        auto phi = std::atan2(-p.z, p.x) + PI;

        u = phi / (2 * PI);
        v = theta / PI;
    }

private:
    Ray center;
    double radius;
    RtMaterial* mat = nullptr;
};
