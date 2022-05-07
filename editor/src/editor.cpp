#include "editor.h"
#include <beet/components.h>
#include <beet/log.h>

namespace beet {
Editor::Editor() {
    m_scene = std::make_unique<Scene>();
    Scene::set_active_scene(*m_scene);
    log::Logger::setup();
    m_engine = std::make_unique<Engine>();

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

        cubeObj.add_component<components::InstanceMesh>("default_cube.obj");

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "UV_Grid_test.png");
        material.set_texture_slot_path(TextureType::Normal, "UV_Grid_test.png");
        material.set_texture_slot_path(TextureType::Metallic, "UV_Grid_test.png");
        material.set_texture_slot_path(TextureType::Roughness, "UV_Grid_test.png");
        material.set_texture_slot_path(TextureType::Occlusion, "UV_Grid_test.png");
        cubeObj.add_component<components::Material>(material);
    }
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
    }
}

}  // namespace beet
