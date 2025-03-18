#include "engine/rt_engine.h"
#include "graphics/rt_renderer.h"

#include "spdlog/spdlog.h"

int main()
{
    RTEngine* engine = new RTEngine();
    RTRenderer* renderer = new RTRenderer();

    sEngineConfiguration configuration;
    configuration.window_width = 1280;
    configuration.window_height = 720;

    if (engine->initialize(renderer, configuration)) {
        return 1;
    }

    renderer->generate_frame();
    renderer->save_frame();

    engine->start_loop();

    engine->clean();

    delete engine;
    delete renderer;

    return 0;
}
