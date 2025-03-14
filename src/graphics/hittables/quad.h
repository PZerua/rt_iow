#pragma once

#include "hittable.h"
#include "hittable_list.h"
#include "graphics/materials/rt_material.h"

class Quad : public Hittable {
public:
    Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, RtMaterial* mat)
        : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = glm::normalize(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        // Compute the bounding box of all four vertices.
        AABB bbox_diagonal1 = AABB(Q, Q + u + v);
        AABB bbox_diagonal2 = AABB(Q + u, Q + v);
        bbox = merge_aabbs(bbox_diagonal1, bbox_diagonal2);
    }

    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override
    {
        double denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        double t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        glm::dvec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        Interval unit_interval = Interval(0, 1);
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    glm::dvec3 Q;
    glm::dvec3 u, v;
    glm::dvec3 w;
    RtMaterial* mat;
    glm::dvec3 normal;
    double D;
};

inline HittableList* box(const glm::dvec3& a, const glm::dvec3& b, RtMaterial* mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    HittableList* sides = new HittableList();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    glm::dvec3 min = glm::dvec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
    glm::dvec3 max = glm::dvec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

    auto dx = glm::dvec3(max.x - min.x, 0, 0);
    auto dy = glm::dvec3(0, max.y - min.y, 0);
    auto dz = glm::dvec3(0, 0, max.z - min.z);

    sides->add(new Quad(glm::dvec3(min.x, min.y, max.z), dx, dy, mat)); // front
    sides->add(new Quad(glm::dvec3(max.x, min.y, max.z), -dz, dy, mat)); // right
    sides->add(new Quad(glm::dvec3(max.x, min.y, min.z), -dx, dy, mat)); // back
    sides->add(new Quad(glm::dvec3(min.x, min.y, min.z), dz, dy, mat)); // left
    sides->add(new Quad(glm::dvec3(min.x, max.y, max.z), dx, -dz, mat)); // top
    sides->add(new Quad(glm::dvec3(min.x, min.y, min.z), dx, dz, mat)); // bottom

    return sides;
}
