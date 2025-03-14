#pragma once

#include "rt_material.h"

#include "graphics/rt_texture.h"

#include "framework/math/math_utils.h"

class DiffuseLight : public RtMaterial {

    RtTexture* tex = nullptr;

public:
    DiffuseLight(const glm::dvec3& albedo) : tex(new SolidColor(albedo)) {}
    DiffuseLight(RtTexture* tex) : tex(tex) {}

    glm::dvec3 emitted(double u, double v, const glm::dvec3& p) const override {
        return tex->value(u, v, p);
    }

};
