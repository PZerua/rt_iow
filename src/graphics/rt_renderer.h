#pragma once

#include "includes.h"

#include "graphics/renderer.h"

#include "graphics/ray.h"

class RTRenderer : public Renderer {

    std::vector<uint8_t> rendered_image;

    glm::dvec3 pixel_delta_u;
    glm::dvec3 pixel_delta_v;

    glm::dvec3 pixel00_loc;

    glm::dvec3 camera_center;

    glm::dvec3 ray_intersect(const Ray& ray);

    void write_color(uint32_t x, uint32_t y, const glm::dvec3& color);

public:

    RTRenderer();

    int initialize(GLFWwindow* window, bool use_mirror_screen = false) override;
    void clean() override;

    void update(float delta_time) override;
    void render() override;

    void generate_frame();
    void save_frame();
};
