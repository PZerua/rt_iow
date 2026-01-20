#include "engine/engine.h"
#include "graphics/rt_renderer.h"

void engine_post_initialize()
{
    Engine* engine = Engine::get_instance();
    RTRenderer* renderer = static_cast<RTRenderer*>(engine->get_renderer());

    renderer->generate_frame();
    renderer->save_frame();
}

void get_engine_config(sEngineConfiguration& out_config)
{
    out_config.window_width = 1280;
    out_config.window_height = 720;

    // Optional callbacks
    out_config.engine_post_initialize = engine_post_initialize;
    out_config.engine_pre_update = nullptr;
    out_config.engine_post_update = nullptr;
    out_config.engine_render = nullptr;
    out_config.custom_renderer_instance = new RTRenderer();
}
