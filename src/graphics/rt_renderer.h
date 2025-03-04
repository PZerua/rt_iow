#pragma once

#include "includes.h"

#include "graphics/renderer.h"
#include "graphics/rt_camera.h"
#include "graphics/ray.h"

#include "hittables/sphere.h"
#include "hittables/hittable_list.h"

class RTRenderer : public Renderer {

    std::vector<uint8_t> rendered_image;

    MeshInstance3D* screen_mesh = nullptr;
    Texture* screen_texture = nullptr;

    sTracingCamera tracing_camera = {};

    HittableList world;

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
