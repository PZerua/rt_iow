#include "rt_renderer.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/renderer_storage.h"

#include "framework/nodes/mesh_instance_3d.h"
#include "framework/camera/camera_2d.h"

#include "shaders/mesh_forward.wgsl.gen.h"

#include "glm/gtx/norm.hpp"

#include <fstream>
#include <iostream>

RTRenderer::RTRenderer() : Renderer()
{

}

int RTRenderer::initialize(GLFWwindow* window, bool use_mirror_screen)
{
    Renderer::initialize(window, use_mirror_screen);

    clear_color = glm::vec4(0.22f, 0.22f, 0.22f, 1.0);

    rendered_image.resize(webgpu_context->screen_width * webgpu_context->screen_height * 4);

    screen_mesh = new MeshInstance3D();

    Surface* screen_surface = new Surface();

    setup_camera();

    // TODO: backface culled!!
    screen_surface->create_quad(2.0f, 2.0f);
    screen_mesh->add_surface(screen_surface);

    camera_2d->set_view(glm::mat4x4(1.0f));
    camera_2d->set_projection(glm::mat4x4(1.0f));
    
    screen_texture = new Texture();
    screen_texture->create(WGPUTextureDimension_2D, WGPUTextureFormat_RGBA8UnormSrgb, { webgpu_context->screen_width, webgpu_context->screen_height, 1 }, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, 1, 1, nullptr);

    Material* screen_material = new Material();
    screen_material->set_is_2D(true);
    screen_material->set_depth_read(false);
    screen_material->set_depth_write(false);
    screen_material->set_type(MATERIAL_UNLIT);
    screen_material->set_diffuse_texture(screen_texture);
    screen_material->set_cull_type(CULL_NONE);
    screen_material->set_shader(RendererStorage::get_shader_from_source(shaders::mesh_forward::source, shaders::mesh_forward::path, screen_material));

    screen_surface->set_material(screen_material);

    world.add(new Sphere(glm::dvec3(0, 0, -1), 0.5));
    world.add(new Sphere(glm::dvec3(0, -100.5, -1), 100));

    return 0;
}

void RTRenderer::setup_camera()
{
    tracing_camera.image_width = webgpu_context->screen_width;
    tracing_camera.image_height = webgpu_context->screen_height;
    tracing_camera.aspect_ratio = (double(tracing_camera.image_width) / tracing_camera.image_height);

    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * tracing_camera.aspect_ratio;
    tracing_camera.center = glm::dvec3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    glm::dvec3 viewport_u = glm::dvec3(viewport_width, 0, 0);
    glm::dvec3 viewport_v = glm::dvec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    tracing_camera.pixel_delta_u = viewport_u / static_cast<double>(tracing_camera.image_width);
    tracing_camera.pixel_delta_v = viewport_v / static_cast<double>(tracing_camera.image_height);

    // Calculate the location of the upper left pixel.
    glm::dvec3 viewport_upper_left = tracing_camera.center - glm::dvec3(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
    tracing_camera.pixel00_loc = viewport_upper_left + 0.5 * (tracing_camera.pixel_delta_u + tracing_camera.pixel_delta_v);

    tracing_camera.samples_per_pixel = 50;
    tracing_camera.pixel_samples_scale = 1.0 / tracing_camera.samples_per_pixel;
}

Ray RTRenderer::get_ray(int x, int y)
{
    // Construct a camera ray originating from the origin and directed at randomly sampled
    // point around the pixel location x, y

    // only apply offset if we want antialiasing, otherwise the aliasing is way worse
    glm::dvec3 offset = tracing_camera.samples_per_pixel > 1 ? sample_square() : glm::dvec3(0.0);
    glm::dvec3 pixel_sample = tracing_camera.pixel00_loc
        + ((x + offset.x) * tracing_camera.pixel_delta_u)
        + ((y + offset.y) * tracing_camera.pixel_delta_v);

    glm::dvec3 ray_origin = tracing_camera.center;
    glm::dvec3 ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
}

glm::dvec3 RTRenderer::sample_square() const
{
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return glm::dvec3(random_d() - 0.5, random_d() - 0.5, 0);
}

void RTRenderer::clean()
{
    Renderer::clean();
}

void RTRenderer::update(float delta_time)
{
    Renderer::update(delta_time);
}

void RTRenderer::render()
{
    add_renderable(screen_mesh, glm::mat4x4(1.0f));

    Renderer::render();
}

glm::dvec3 RTRenderer::ray_intersect(const Ray& ray, const Hittable& world)
{
    hit_record rec;
    if (world.hit(ray, { 0, infinity }, rec)) {
        return 0.5 * (rec.normal + glm::dvec3(1, 1, 1));
    }

    glm::dvec3 unit_direction = glm::normalize(ray.direction());
    auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * glm::dvec3(1.0, 1.0, 1.0) + a * glm::dvec3(0.5, 0.7, 1.0);
}

void RTRenderer::write_color(uint32_t x, uint32_t y, const glm::dvec3& color)
{
    glm::dvec3 clampled_color = glm::clamp(color);

    uint8_t r8 = static_cast<uint8_t>(clampled_color.r * 255.999);
    uint8_t g8 = static_cast<uint8_t>(clampled_color.g * 255.999);
    uint8_t b8 = static_cast<uint8_t>(clampled_color.b * 255.999);

    rendered_image[x * 4 + 0 + y * webgpu_context->screen_width * 4] = r8;
    rendered_image[x * 4 + 1 + y * webgpu_context->screen_width * 4] = g8;
    rendered_image[x * 4 + 2 + y * webgpu_context->screen_width * 4] = b8;
}

void RTRenderer::generate_frame()
{
    for (int y = 0; y < webgpu_context->screen_height; y++) {
        std::clog << "\rScanlines remaining: " << (webgpu_context->screen_height - y) << ' ' << std::flush;
        for (int x = 0; x < webgpu_context->screen_width; x++) {
            glm::dvec3 pixel_color = glm::dvec3(0.0);

            for (int sample = 0; sample < tracing_camera.samples_per_pixel; sample++) {
                Ray ray = get_ray(x, y);
                pixel_color += ray_intersect(ray, world);
            }

            write_color(x, y, pixel_color * tracing_camera.pixel_samples_scale);
        }
    }
    std::clog << "\rDone.                 \n";

    screen_texture->update(rendered_image.data(), 0, {});
}

void RTRenderer::save_frame()
{
    std::ofstream saved_frame;
    saved_frame.open("data/frame.ppm");

    saved_frame << "P3\n" << webgpu_context->screen_width << ' ' << webgpu_context->screen_height << "\n255\n";

    for (int y = 0; y < webgpu_context->screen_height; y++) {
        for (int x = 0; x < webgpu_context->screen_width; x++) {

            uint32_t idx = x * 4 + y * webgpu_context->screen_width * 4;

            uint8_t r8 = rendered_image[idx + 0];
            uint8_t g8 = rendered_image[idx + 1];
            uint8_t b8 = rendered_image[idx + 2];

            saved_frame << std::to_string(r8) << ' ' << std::to_string(g8) << ' ' << std::to_string(b8) << '\n';
        }
    }

    saved_frame.close();
}

void RTRenderer::resize_window(int width, int height)
{
    Renderer::resize_window(width, height);

    camera_2d->set_view(glm::mat4x4(1.0f));
    camera_2d->set_projection(glm::mat4x4(1.0f));

    // To recreate on resize
    {
        rendered_image.resize(webgpu_context->screen_width * webgpu_context->screen_height * 4);
        screen_texture->create(WGPUTextureDimension_2D, WGPUTextureFormat_RGBA8UnormSrgb, { webgpu_context->screen_width, webgpu_context->screen_height, 1 }, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, 1, 1, nullptr);
        screen_mesh->get_surface_material(0)->set_dirty_flag(PROP_DIFFUSE_TEXTURE);

        setup_camera();

        generate_frame();
        save_frame();
    }
}
