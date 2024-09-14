#include "rt_engine.h"

#include "framework/nodes/environment_3d.h"

#include "graphics/rt_renderer.h"
#include "graphics/renderer_storage.h"

#include "engine/scene.h"

#include "shaders/mesh_grid.wgsl.gen.h"

int RTEngine::initialize(Renderer* renderer, sEngineConfiguration configuration)
{
    int error = Engine::initialize(renderer, configuration);

    if (error) return error;

    main_scene = new Scene("main_scene");

	return error;
}

void RTEngine::clean()
{
    Engine::clean();
}

void RTEngine::update(float delta_time)
{
    Engine::update(delta_time);

    main_scene->update(delta_time);
}

void RTEngine::render()
{
    //render_default_gui();

    main_scene->render();

    Engine::render();
}
