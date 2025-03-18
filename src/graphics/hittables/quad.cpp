#include "quad.h"

bool Quad::hit(const Ray& ray, const Interval& ray_t, hit_record& rec) const
{
    glm::mat4x4 model = transform.get_model();

    Ray ray_local(inv_model * glm::dvec4(ray.origin(), 1.0), inv_model * glm::dvec4(ray.direction(), 0.0), ray.time());

    double denom = dot(normal, ray_local.direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denom) < 1e-8)
        return false;

    // Return false if the hit point parameter t is outside the ray interval.
    double t = (D - dot(normal, ray_local.origin())) / denom;
    if (!ray_t.contains(t))
        return false;

    // Determine if the hit point lies within the planar shape using its plane coordinates.
    auto intersection = ray_local.at(t);
    glm::dvec3 planar_hitpt_vector = intersection - Q;
    auto alpha = dot(w, cross(planar_hitpt_vector, v));
    auto beta = dot(w, cross(u, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
        return false;

    // Ray hits the 2D shape; set the rest of the hit record and return true.

    rec.t = t;
    rec.p = model * glm::dvec4(intersection, 1.0);
    rec.mat = mat;
    rec.set_face_normal(ray_local, normal);

    rec.normal = model * glm::dvec4(rec.normal, 0.0);

    return true;
}
