#pragma once

#include "hittables/hittable.h"

#include <vector>

#include "glm/glm.hpp"

struct sTracingCamera {
    int         image_width = 1280; // Rendered image width
    int         image_height = 720; // Rendered image height
    double      aspect_ratio;       // Ratio of image width over height

    int         samples_per_pixel = 10; // Count of random samples for each pixel

    int         max_depth = 10; // Maximum number of ray bounces into scene

    glm::dvec3  background = {}; // Scene background color

    double      vfov = 90; // Vertical view angle (field of view)

    glm::dvec3 lookfrom = glm::dvec3(0, 0, 0); // Point camera is looking from
    glm::dvec3 lookat = glm::dvec3(0, 0, -1);  // Point camera is looking at
    glm::dvec3 vup = glm::dvec3(0, 1, 0);      // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void initialize();

    void render(const Hittable& world, std::vector<uint8_t>& rendered_image);
    void write_color(std::vector<uint8_t>& rendered_image, uint32_t x, uint32_t y, const glm::dvec3& color);

    glm::dvec3 ray_intersect(const Ray& ray, int depth, const Hittable& world);

private:

    Ray get_ray(int x, int y);
    glm::dvec3 sample_square() const;
    glm::dvec3 defocus_disk_sample() const;

    double      pixel_samples_scale; // Color scale factor for a sum of pixel samples
    glm::dvec3  center;        // Camera center
    glm::dvec3  pixel00_loc;   // Location of pixel 0, 0
    glm::dvec3  pixel_delta_u; // Offset to pixel to the right
    glm::dvec3  pixel_delta_v; // Offset to pixel below
    glm::dvec3  u, v, w;       // Camera frame basis vectors
    glm::dvec3  defocus_disk_u; // Defocus disk horizontal radius
    glm::dvec3  defocus_disk_v; // Defocus disk vertical radius
};
