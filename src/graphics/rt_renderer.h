#pragma once

#include "includes.h"

#include "graphics/renderer.h"

#include "graphics/ray.h"

#include "hittables/sphere.h"
#include "hittables/hittable_list.h"

struct sTracingCamera {
    double      aspect_ratio;  // Ratio of image width over height
    int         image_width;   // Rendered image width in pixel count
    int         image_height;  // Rendered image height
    glm::dvec3  center;        // Camera center
    glm::dvec3  pixel00_loc;   // Location of pixel 0, 0
    glm::dvec3  pixel_delta_u; // Offset to pixel to the right
    glm::dvec3  pixel_delta_v; // Offset to pixel below

    int         samples_per_pixel;   // Count of random samples for each pixel
    double      pixel_samples_scale; // Color scale factor for a sum of pixel samples

    int         max_depth = 10; // Maximum number of ray bounces into scene
};

class RTRenderer : public Renderer {

    std::vector<uint8_t> rendered_image;

    MeshInstance3D* screen_mesh = nullptr;
    Texture* screen_texture = nullptr;

    sTracingCamera tracing_camera = {};

    Hittable_list world;

    glm::dvec3 ray_intersect(const Ray& ray, int depth, const Hittable& world);

    double linear_to_gamma(double linear_component);

    void write_color(uint32_t x, uint32_t y, const glm::dvec3& color);

    void setup_camera();

    Ray get_ray(int x, int y);
    glm::dvec3 sample_square() const;

public:

    RTRenderer();

    int pre_initialize(GLFWwindow* window, bool use_mirror_screen = false) override;
    int initialize() override;
    int post_initialize() override;

    void clean() override;

    void update(float delta_time) override;
    void render() override;

    void generate_frame();
    void save_frame();

    virtual void resize_window(int width, int height) override;

};
