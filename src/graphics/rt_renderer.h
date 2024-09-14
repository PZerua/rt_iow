#pragma once

#include "includes.h"

#include "graphics/renderer.h"

class RTRenderer : public Renderer {

public:

    RTRenderer();

    int initialize(GLFWwindow* window, bool use_mirror_screen = false) override;
    void clean() override;

    void update(float delta_time) override;
    void render() override;
};
