#include "sphere.h"

bool Sphere::hit(const Ray& ray, const Interval& interval, hit_record& rec) const
{
    glm::mat4x4 model = transform.get_model();

    Ray ray_local(inv_model * glm::dvec4(ray.origin(), 1.0), inv_model * glm::dvec4(ray.direction(), 0.0), ray.time());

    glm::dvec3 current_center = center.at(ray_local.time());
    glm::dvec3 oc = current_center - ray_local.origin();
    double a = glm::length2(ray_local.direction());
    double h = dot(ray_local.direction(), oc);
    double c = glm::length2(oc) - radius * radius;

    double discriminant = h * h - a * c;
    if (discriminant < 0)
        return false;

    double sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    double root = (h - sqrtd) / a;
    if (!interval.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!interval.surrounds(root))
            return false;
    }

    rec.t = root;
    glm::dvec3 local_p = ray_local.at(rec.t);
    rec.p = model * glm::dvec4(local_p, 1.0);

    rec.mat = mat;

    glm::dvec3 outward_normal = (local_p - current_center) / radius;

    rec.set_face_normal(ray_local, outward_normal);

    rec.normal = model * glm::dvec4(rec.normal, 0.0);

    get_sphere_uv(rec.normal, rec.u, rec.v);

    return true;
}
