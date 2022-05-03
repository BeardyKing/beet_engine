#include "editor.h"
#include <beet/components.h>
#include <beet/log.h>

namespace beet {
Editor::Editor() {
    m_scene = std::make_unique<Scene>();
    Scene::set_active_scene(*m_scene);
    log::Logger::setup();
    m_engine = std::make_unique<Engine>();

    //=Scene setup===

    {
        auto editorCamera = m_scene->create_game_object("camera");
        auto& cam = editorCamera.add_component<components::Camera>();
        editorCamera.get_component<components::Transform>().set_position(glm::vec3(0, 0, -5));
    }

    {
        auto cubeObj = m_scene->create_game_object("loaded_cube");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, 0, 0));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 45, 0));

        auto shader = components::ShaderProgram();
        shader.set_asset_name("fallback");
        shader.load_shader("fallback", "fallback.vert", "fallback.frag");

        cubeObj.add_component<components::Mesh>("default_cube.obj").on_awake();
        cubeObj.add_component<components::Texture>("UV_Grid_test.png").on_awake();
        cubeObj.add_component<components::ShaderProgram>(shader).on_awake();
    }
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
    }
}

}  // namespace beet
