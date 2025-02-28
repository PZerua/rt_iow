#pragma once

#include "glm/vec3.hpp"

class Ray {
public:
    Ray() {}

    Ray(const glm::dvec3& origin, const glm::dvec3& direction) :
        orig(origin), dir(direction), tm(0.0)
    {}

    Ray(const glm::dvec3& origin, const glm::dvec3& direction, double time) :
        orig(origin), dir(direction), tm(time)
    {}

    const glm::dvec3& origin() const { return orig; }
    const glm::dvec3& direction() const { return dir; }
    double time() const { return tm; }

    glm::dvec3 at(double t) const {
        return orig + t * dir;
    }

private:
    glm::dvec3 orig;
    glm::dvec3 dir;
    double tm;
};
