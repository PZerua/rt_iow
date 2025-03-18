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

#include "graphics/scenes.h"

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
    // scenes::three_spheres(world, tracing_camera);

    // scene multiple spheres
    //scenes::bouncing_spheres(world, tracing_camera);

    // scene multiple spheres
    //scenes::checkered_spheres(world, tracing_camera);

    // scene multiple spheres
    //scenes::earth(world, tracing_camera);

    //scenes::quads(world, tracing_camera);

    // scenes::simple_light(world, tracing_camera);

    scenes::cornell_box(world, tracing_camera);

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
