#include "sphere.h"

bool Sphere::hit(const Ray& ray, const Interval& interval, hit_record& rec) const
{
    glm::dvec3 oc = center - ray.origin();
    auto a = glm::length2(ray.direction());
    auto h = dot(ray.direction(), oc);
    auto c = glm::length2(oc) - radius * radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
        return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!interval.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!interval.surrounds(root))
            return false;
    }

    rec.t = root;
    rec.p = ray.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    glm::dvec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(ray, outward_normal);

    return true;
}
