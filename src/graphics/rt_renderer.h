#pragma once

#include "includes.h"

#include "graphics/renderer.h"

#include "graphics/ray.h"

#include "hittables/sphere.h"
#include "hittables/hittable_list.h"

class RTRenderer : public Renderer {

    std::vector<uint8_t> rendered_image;

    glm::dvec3 pixel_delta_u;
    glm::dvec3 pixel_delta_v;

    glm::dvec3 pixel00_loc;

    glm::dvec3 camera_center;

    MeshInstance3D* screen_mesh;
    Texture* screen_texture;

    Hittable_list world;

    glm::dvec3 ray_intersect(const Ray& ray, const Hittable& world);

    void write_color(uint32_t x, uint32_t y, const glm::dvec3& color);

public:

    RTRenderer();

    int initialize(GLFWwindow* window, bool use_mirror_screen = false) override;
    void clean() override;

    void update(float delta_time) override;
    void render() override;

    double hit_sphere(const glm::dvec3& center, double radius, const Ray& r);

    void generate_frame();
    void save_frame();
};
