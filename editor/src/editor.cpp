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
        material.set_texture_slot_path(TextureType::Normal, "normal_tiles_1k.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);
    }

    {
        auto light = m_scene->create_game_object("loaded_cube");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(1, 0, 1));
        pointLight.set_color(vec3(1, 0, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
    }
    {
        auto light = m_scene->create_game_object("loaded_cube");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(0, 0, 1));
        pointLight.set_color(vec3(0, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
    }
    {
        auto light = m_scene->create_game_object("loaded_cube");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(1, 0, 0));
        pointLight.set_color(vec3(0, 0, 1));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
    }
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
    }
}

}  // namespace beet
