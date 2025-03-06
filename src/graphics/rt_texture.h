#pragma once

#include "glm/glm.hpp"
#include "graphics/texture.h"

class RtTexture {
public:
    virtual ~RtTexture() = default;

    virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) const = 0;
};

class SolidColor : public RtTexture {
public:
    SolidColor(const glm::dvec3& albedo) : albedo(albedo) {}

    SolidColor(double red, double green, double blue) : SolidColor(glm::dvec3(red, green, blue)) {}

    glm::dvec3 value(double u, double v, const glm::dvec3& p) const override {
        return albedo;
    }

private:
    glm::dvec3 albedo;
};

class CheckerTexture : public RtTexture {
public:
    CheckerTexture(double scale, RtTexture* even, RtTexture* odd)
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

    CheckerTexture(double scale, const glm::dvec3& c1, const glm::dvec3& c2)
        : CheckerTexture(scale, new SolidColor(c1), new SolidColor(c2)) {}

    glm::dvec3 value(double u, double v, const glm::dvec3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x));
        auto yInteger = int(std::floor(inv_scale * p.y));
        auto zInteger = int(std::floor(inv_scale * p.z));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    RtTexture* even;
    RtTexture* odd;
};

class ImageTexture : public RtTexture {
public:
    ImageTexture(const char* filename) {
        image.load(filename, false, false, true);
    }

    glm::dvec3 value(double u, double v, const glm::dvec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.

        const sTextureData& texture_data = image.get_texture_data();

        if (texture_data.image_height <= 0) return glm::dvec3(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);  // Flip V to image coordinates

        int i = int(u * texture_data.image_width);
        int j = int(v * texture_data.image_height);
        const uint8_t* pixel = texture_data.pixel_data(i, j);

        float color_scale = 1.0 / 255.0;
        return glm::dvec3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    Texture image;
};
