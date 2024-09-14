#include "rt_renderer.h"

#include "graphics/shader.h"

RTRenderer::RTRenderer() : Renderer()
{

}

int RTRenderer::initialize(GLFWwindow* window, bool use_mirror_screen)
{
    Renderer::initialize(window, use_mirror_screen);

    clear_color = glm::vec4(0.22f, 0.22f, 0.22f, 1.0);

    return 0;
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
    Renderer::render();
}
