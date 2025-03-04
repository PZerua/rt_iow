#include "rt_renderer.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/renderer_storage.h"
#include "graphics/hittables/bvh_node.h"

#include "graphics/materials/lambertian_material.h"
#include "graphics/materials/metallic_material.h"
#include "graphics/materials/dielectric_material.h"

#include "framework/nodes/mesh_instance_3d.h"
#include "framework/camera/camera_2d.h"
#include "framework/utils/timer.h"

#include "shaders/mesh_forward.wgsl.gen.h"

#include "glm/gtx/norm.hpp"

#include "spdlog/spdlog.h"

#include <fstream>

RTRenderer::RTRenderer() : Renderer()
{

}

int RTRenderer::pre_initialize(GLFWwindow* window, bool use_mirror_screen)
{
    return Renderer::pre_initialize(window, use_mirror_screen);
}

int RTRenderer::initialize()
{
    int error_code = Renderer::initialize();

    clear_color = glm::vec4(0.22f, 0.22f, 0.22f, 1.0);

    return error_code;
}

int RTRenderer::post_initialize()
{
    Renderer::post_initialize();

    rendered_image.resize(webgpu_context->screen_width * webgpu_context->screen_height * 4);

    // quad mesh to show gpu texture on window
    Surface* screen_surface = new Surface();
    screen_surface->create_quad(2.0f, 2.0f);

    screen_mesh = new MeshInstance3D();
    screen_mesh->add_surface(screen_surface);

    camera_2d->set_view(glm::mat4x4(1.0f));
    camera_2d->set_projection(glm::mat4x4(1.0f));

    // gpu texture to store the generated frame
    screen_texture = new Texture();
    screen_texture->create(WGPUTextureDimension_2D, WGPUTextureFormat_RGBA8UnormSrgb, { webgpu_context->screen_width, webgpu_context->screen_height, 1 }, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, 1, 1, nullptr);

    // material to show generated frame on window
    Material* screen_material = new Material();
    screen_material->set_is_2D(true);
    screen_material->set_depth_read(false);
    screen_material->set_depth_write(false);
    screen_material->set_type(MATERIAL_UNLIT);
    screen_material->set_diffuse_texture(screen_texture);
    screen_material->set_shader(RendererStorage::get_shader_from_source(shaders::mesh_forward::source, shaders::mesh_forward::path, shaders::mesh_forward::libraries, screen_material));

    screen_surface->set_material(screen_material);

    tracing_camera.image_width = webgpu_context->screen_width;
    tracing_camera.image_height = webgpu_context->screen_height;

    // scene 3 spheres
    //{
    //    RTMaterial* material_ground = new LambertianMaterial({ 0.8, 0.8, 0.0 });
    //    RTMaterial* material_center = new LambertianMaterial({ 0.1, 0.2, 0.5 });
    //    RTMaterial* material_left = new DielectricMaterial(1.5);
    //    RTMaterial* material_bubble = new DielectricMaterial(1.0 / 1.5);
    //    RTMaterial* material_right = new MetallicMaterial({ 0.8, 0.6, 0.2 }, 1.0);

    //    world.add(new Sphere(glm::dvec3(0.0, -100.5, -1.0), 100.0, material_ground));
    //    world.add(new Sphere(glm::dvec3(0.0, 0.0, -1.2), 0.5, material_center));
    //    world.add(new Sphere(glm::dvec3(-1.0, 0.0, -1.0), 0.5, material_left));
    //    world.add(new Sphere(glm::dvec3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    //    world.add(new Sphere(glm::dvec3(1.0, 0.0, -1.0), 0.5, material_right));

    //    tracing_camera.lookfrom = glm::dvec3(0.0);
    //    tracing_camera.lookat = glm::dvec3(0.0, 0.0, -1.0);
    //    tracing_camera.vup = glm::dvec3(0.0, 1.0, 0.0);

    //    tracing_camera.samples_per_pixel = 25;

    //    tracing_camera.vfov = 90;
    //}

    // scene multiple spheres
    {
        RTMaterial* material_ground = new LambertianMaterial({ 0.5, 0.5, 0.5 });
        world.add(new Sphere(glm::dvec3(0.0, -1000.0, -1.0), 1000.0, material_ground));

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                double choose_mat = random_d();
                glm::dvec3 center(a + 0.9 * random_d(), 0.2, b + 0.9 * random_d());

                if (glm::length((center - glm::dvec3(4, 0.2, 0))) > 0.9) {
                    RTMaterial* sphere_material;

                    if (choose_mat < 0.8) {
                        // diffuse
                        auto albedo = random_color() * random_color();
                        sphere_material = new LambertianMaterial(albedo);
                        auto center2 = center + glm::dvec3(0, random_d(0, .5), 0);
                        world.add(new Sphere(center, center2, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = random_color(0.5, 1);
                        auto fuzz = random_d(0, 0.5);
                        sphere_material = new MetallicMaterial(albedo, fuzz);
                        world.add(new Sphere(center, 0.2, sphere_material));
                    }
                    else {
                        // glass
                        sphere_material = new DielectricMaterial(1.5);
                        world.add(new Sphere(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = new DielectricMaterial(1.5);
        world.add(new Sphere({ 0, 1, 0 }, 1.0, material1));

        auto material2 = new LambertianMaterial({ 0.4, 0.2, 0.1 });
        world.add(new Sphere({ -4, 1, 0 }, 1.0, material2));

        auto material3 = new MetallicMaterial({ 0.7, 0.6, 0.5 }, 0.0);
        world.add(new Sphere({ 4, 1, 0 }, 1.0, material3));

        tracing_camera.samples_per_pixel = 10;
        tracing_camera.max_depth = 50;

        tracing_camera.vfov = 20;
        tracing_camera.lookfrom = glm::dvec3(13, 2, 3);
        tracing_camera.lookat = glm::dvec3(0, 0, 0);
        tracing_camera.vup = glm::dvec3(0.0, 1.0, 0.0);

        tracing_camera.defocus_angle = 0.6;
        tracing_camera.focus_dist = 10.0;
    }

    world = HittableList(new BVHNode(world));

    tracing_camera.initialize();

    return 0;
}

void RTRenderer::clean()
{
    Renderer::clean();

    for (Hittable* hittable : world.objects) {
        delete hittable;
    }
}

void RTRenderer::update(float delta_time)
{
    Renderer::update(delta_time);
}

void RTRenderer::render()
{
    screen_mesh->render();

    Renderer::render();
}

void RTRenderer::generate_frame()
{
    spdlog::info("Generate Frame");

    Timer frame_time;
    frame_time.start();

    tracing_camera.render(world, rendered_image);

    frame_time.print_elapsed_time_s();

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

        tracing_camera.image_width = webgpu_context->screen_width;
        tracing_camera.image_height = webgpu_context->screen_height;
        tracing_camera.initialize();

        generate_frame();
        save_frame();
    }
}
