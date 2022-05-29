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
        editorCamera.get_component<components::Transform>().set_position(glm::vec3(0.0f, 1.125f, -6.5));
        cam.set_look_target(editorCamera.get_component<components::Transform>().get_position() +
                            editorCamera.get_component<components::Transform>().forward());
    }

    {
        auto cubeObj = m_scene->create_game_object("loaded_cube");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, 0, 0));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 45, 0));

        cubeObj.add_component<components::InstanceMesh>("cube");

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "darkOceanTiles07/DarkOceanTiles07_1K_Albedo.png");
        material.set_texture_slot_path(TextureType::Normal, "darkOceanTiles07/DarkOceanTiles07_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "darkOceanTiles07/DarkOceanTiles07_1K_Height.png");
        material.set_texture_slot_path(TextureType::Roughness, "darkOceanTiles07/DarkOceanTiles07_1K_Roughness.png");
        material.set_texture_slot_path(TextureType::Occlusion, "darkOceanTiles07/DarkOceanTiles07_1K_AO.png");
        cubeObj.add_component<components::Material>(material);
    }

    {
        auto cubeObj = m_scene->create_game_object("red glass");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(-0.2, -0.2, -1.2));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));

        cubeObj.add_component<components::InstanceMesh>("plane");

        auto material = components::Material();
        material.temp_set_shader("transparency", "transparent.vert", "transparent.frag", "transparent");

        material.set_is_opaque(false);
        material.set_albedo_color(vec4(1.0f, 0.0f, 0.0f, 0.5f));
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");

        cubeObj.add_component<components::Material>(material);
    }

    {
        auto cubeObj = m_scene->create_game_object("green glass");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(-0.4, -0.4, -1.4));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));

        cubeObj.add_component<components::InstanceMesh>("plane");

        auto material = components::Material();
        material.temp_set_shader("transparency", "transparent.vert", "transparent.frag", "transparent");
        material.set_is_opaque(false);
        material.set_albedo_color(vec4(0.0f, 1.0f, 0.0f, 0.5f));
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");

        cubeObj.add_component<components::Material>(material);
    }

    {
        auto cubeObj = m_scene->create_game_object("blue glass");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(-0.6, -0.6, -1.6));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));

        cubeObj.add_component<components::InstanceMesh>("plane");

        auto material = components::Material();
        material.temp_set_shader("transparency", "transparent.vert", "transparent.frag", "transparent");
        material.set_is_opaque(false);
        material.set_albedo_color(vec4(0.0f, 0.0f, 1.0f, 0.5f));
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");

        cubeObj.add_component<components::Material>(material);
    }

    {
        auto light = m_scene->create_game_object("pLight0");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(-2, -1, 1));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(0, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        light.add_component<components::Material>(material);

        light.add_component<components::InstanceMesh>("sphere.fbx");
    }
    {
        auto light = m_scene->create_game_object("pLight1");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(-1.5f, 0, -1.5f));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));

        pointLight.set_color(vec3(0, 1, 1));
        pointLight.set_intensity(3);
        pointLight.set_range(10);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        light.add_component<components::Material>(material);

        light.add_component<components::InstanceMesh>("sphere.fbx");
    }
    {
        auto light = m_scene->create_game_object("pLight2");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(1.5f));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(1, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        light.add_component<components::Material>(material);

        light.add_component<components::InstanceMesh>("sphere.fbx");
    }

    {
        auto postProcessing = m_scene->create_game_object("Post processing");
        postProcessing.add_component<components::PostProcessing>();
    }

    auto cubeObj = m_scene->create_game_object("skybox");
    auto skybox = components::SkyBox();
    skybox.set_texture_slot_path(SkyBoxTextureType::SkyBox, "skybox/cmft_skybox.hdr");
    skybox.set_texture_slot_path(SkyBoxTextureType::Irradiance, "skybox/cmtr_irradiance.hdr");
    skybox.set_texture_slot_path(SkyBoxTextureType::Radiance, "skybox/cmtr_radiance.hdr");
    cubeObj.add_component<components::SkyBox>(skybox);
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
    }
}

}  // namespace beet
