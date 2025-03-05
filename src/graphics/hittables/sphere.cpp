#include "sphere.h"

bool Sphere::hit(const Ray& ray, const Interval& interval, hit_record& rec) const
{
    glm::dvec3 current_center = center.at(ray.time());
    glm::dvec3 oc = current_center - ray.origin();
    double a = glm::length2(ray.direction());
    double h = dot(ray.direction(), oc);
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
    rec.p = ray.at(rec.t);
    rec.mat = mat;

    glm::dvec3 outward_normal = (rec.p - current_center) / radius;
    rec.set_face_normal(ray, outward_normal);

    get_sphere_uv(outward_normal, rec.u, rec.v);

    return true;
}
