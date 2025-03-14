#include "rt_camera.h"

#include "graphics/materials/rt_material.h"

#include "graphics/utils.h"

#include <iostream>

void sTracingCamera::initialize()
{
    aspect_ratio = (double(image_width) / image_height);

    center = lookfrom;

    // Camera
    auto theta = glm::radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    double viewport_width = viewport_height * aspect_ratio;

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = glm::normalize(lookfrom - lookat);
    u = glm::normalize(glm::cross(vup, w));
    v = glm::cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    glm::dvec3 viewport_u = viewport_width * u;
    glm::dvec3 viewport_v = viewport_height * -v;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / static_cast<double>(image_width);
    pixel_delta_v = viewport_v / static_cast<double>(image_height);

    // Calculate the location of the upper left pixel.
    glm::dvec3 viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0 - viewport_v / 2.0;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    double defocus_radius = focus_dist * std::tan(glm::radians(defocus_angle / 2.0));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;

    pixel_samples_scale = 1.0 / samples_per_pixel;
}

void sTracingCamera::render(const Hittable& world, std::vector<uint8_t>& rendered_image)
{
    for (int y = 0; y < image_height; y++) {
#ifndef __EMSCRIPTEN__
        std::clog << "\rScanlines remaining: " << (image_height - y) << ' ' << std::flush;
#endif
        for (int x = 0; x < image_width; x++) {
            glm::dvec3 pixel_color = glm::dvec3(0.0);

            for (int sample = 0; sample < samples_per_pixel; sample++) {
                Ray ray = get_ray(x, y);
                pixel_color += ray_intersect(ray, max_depth, world);
            }

            write_color(rendered_image, x, y, pixel_color * pixel_samples_scale);
        }
    }

#ifndef __EMSCRIPTEN__
    std::clog << "\rDone.                 \n";
#endif

}

void sTracingCamera::write_color(std::vector<uint8_t>& rendered_image, uint32_t x, uint32_t y, const glm::dvec3& color)
{
    glm::dvec3 clampled_color = glm::clamp(color);

    uint8_t r8 = static_cast<uint8_t>(linear_to_gamma(clampled_color.r) * 255.999);
    uint8_t g8 = static_cast<uint8_t>(linear_to_gamma(clampled_color.g) * 255.999);
    uint8_t b8 = static_cast<uint8_t>(linear_to_gamma(clampled_color.b) * 255.999);

    rendered_image[x * 4 + 0 + y * image_width * 4] = r8;
    rendered_image[x * 4 + 1 + y * image_width * 4] = g8;
    rendered_image[x * 4 + 2 + y * image_width * 4] = b8;
}

glm::dvec3 sTracingCamera::ray_intersect(const Ray& ray, int depth, const Hittable& world)
{
    if (depth <= 0) {
        return { 0.0, 0.0, 0.0 };
    }

    hit_record rec;

    // If the ray hits nothing, return the background color.
    if (!world.hit(ray, { 0.001, infinity }, rec))
        return background;

    Ray scattered;
    glm::dvec3 attenuation;
    glm::dvec3 color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat->scatter(ray, rec, attenuation, scattered)) {
        return color_from_emission;
    }

    glm::dvec3 color_from_scatter = attenuation * ray_intersect(scattered, depth - 1, world);

    return color_from_emission + color_from_scatter;
}

Ray sTracingCamera::get_ray(int x, int y)
{
    // Construct a camera ray originating from the defocus disk and directed at a randomly
    // sampled point around the pixel location x, y

    // only apply offset if we want antialiasing, otherwise the aliasing is way worse
    glm::dvec3 offset = samples_per_pixel > 1 ? sample_square() : glm::dvec3(0.0);
    glm::dvec3 pixel_sample = pixel00_loc
        + ((x + offset.x) * pixel_delta_u)
        + ((y + offset.y) * pixel_delta_v);

    glm::dvec3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    glm::dvec3 ray_direction = pixel_sample - ray_origin;
    double ray_time = random_d();

    return Ray(ray_origin, ray_direction, ray_time);
}

glm::dvec3 sTracingCamera::sample_square() const
{
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return glm::dvec3(random_d() - 0.5, random_d() - 0.5, 0);
}

glm::dvec3 sTracingCamera::defocus_disk_sample() const
{
    // Returns a random point in the camera defocus disk.
    auto p = random_point_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}
