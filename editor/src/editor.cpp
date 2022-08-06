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
        auto cubeObj = m_scene->create_game_object("54k glass");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(3.5, -3.5, -3.5));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(0.1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));

        cubeObj.add_component<components::InstanceMesh>("30x30x30_54k_tris.fbx");

        auto material = components::Material();
        material.temp_set_shader("oit", "oit.vert", "oit.frag", "oit");

        material.set_is_opaque(false);
        material.set_albedo_color(vec4(100.0f / 255.0f, 100.0f / 255.0f, 255.0f / 255.0f, 64.0f / 255.0f));
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");

        cubeObj.add_component<components::Material>(material);
    }

    {
        auto cubeObj = m_scene->create_game_object("stanford dragon");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, -3, 0));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(5));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 270, 0));

        cubeObj.add_component<components::InstanceMesh>("dragon.fbx");

        auto material = components::Material();
        material.temp_set_shader("oit", "oit.vert", "oit.frag", "oit");
        material.set_albedo_scalar(1.0f);
        material.set_normal_scalar(1.29);
        material.set_metallic_scalar(1.0f);
        material.set_roughness_scalar(2.6f);
        material.set_occlusion_scalar(1.0f);

        material.set_is_opaque(false);
        material.set_albedo_color(vec4(1.0, 1.0, 1.0, 0.5));
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");

        cubeObj.add_component<components::Material>(material);
    }

//    {
//        auto cubeObj = m_scene->create_game_object("loaded_cube");
//        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, -4, 0));
//        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
//        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 45, 0));
//
//        cubeObj.add_component<components::InstanceMesh>("cube");
//
//        auto material = components::Material();
//        material.set_texture_slot_path(TextureType::Albedo, "darkOceanTiles07/DarkOceanTiles07_1K_Albedo.png");
//        material.set_texture_slot_path(TextureType::Normal, "darkOceanTiles07/DarkOceanTiles07_1K_Normal.png");
//        material.set_texture_slot_path(TextureType::Metallic, "darkOceanTiles07/DarkOceanTiles07_1K_Height.png");
//        material.set_texture_slot_path(TextureType::Roughness, "darkOceanTiles07/DarkOceanTiles07_1K_Roughness.png");
//        material.set_texture_slot_path(TextureType::Occlusion, "darkOceanTiles07/DarkOceanTiles07_1K_AO.png");
//        cubeObj.add_component<components::Material>(material);
//    }
//
//
//
//    {
//        auto cubeObj = m_scene->create_game_object("red glass");
//        cubeObj.get_component<components::Transform>().set_position(glm::vec3(7, -0.2, -1.1));
//        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, .1f));
//        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));
//
//        cubeObj.add_component<components::InstanceMesh>("cube");
//
//        auto material = components::Material();
//        material.temp_set_shader("oit", "oit.vert", "oit.frag", "oit");
//
//        material.set_is_opaque(false);
//        material.set_albedo_color(vec4(0.9f, 0.2f, 0.2f, 0.55f));
//        material.set_texture_slot_path(TextureType::Albedo, "alpha_test.png");
//        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
//
//        cubeObj.add_component<components::Material>(material);
//    }
//
//    {
//        auto cubeObj = m_scene->create_game_object("green glass");
//        cubeObj.get_component<components::Transform>().set_position(glm::vec3(7, -0.4, -1.4));
//        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, .1f));
//        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));
//
//        cubeObj.add_component<components::InstanceMesh>("cube");
//
//        auto material = components::Material();
//        material.temp_set_shader("oit", "oit.vert", "oit.frag", "oit");
//
//        material.set_is_opaque(false);
//        material.set_albedo_color(vec4(0.2f, 0.9f, 0.2f, 0.55f));
//        material.set_texture_slot_path(TextureType::Albedo, "alpha_test.png");
//        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
//
//        cubeObj.add_component<components::Material>(material);
//    }
//
//    {
//        auto cubeObj = m_scene->create_game_object("blue glass");
//        cubeObj.get_component<components::Transform>().set_position(glm::vec3(7, -0.6, -1.7));
//        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, .1f));
//        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 180, 0));
//
//        cubeObj.add_component<components::InstanceMesh>("cube");
//
//        auto material = components::Material();
//        material.temp_set_shader("oit", "oit.vert", "oit.frag", "oit");
//
//        material.set_is_opaque(false);
//        material.set_albedo_color(vec4(0.2f, 0.2f, 0.9f, 0.55f));
//        material.set_texture_slot_path(TextureType::Albedo, "alpha_test.png");
//        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
//
//        cubeObj.add_component<components::Material>(material);
//    }
//
//    {
//        auto light = m_scene->create_game_object("pLight0");
//        auto& pointLight = light.add_component<components::PointLight>();
//        light.get_component<components::Transform>().set_position(glm::vec3(-2, -1, 1));
//        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
//        pointLight.set_color(vec3(0, 1, 0));
//        pointLight.set_intensity(3);
//        pointLight.set_range(10);
//
//        auto material = components::Material();
//        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
//        light.add_component<components::Material>(material);
//
//        light.add_component<components::InstanceMesh>("sphere.fbx");
//    }
//    {
//        auto light = m_scene->create_game_object("pLight1");
//        auto& pointLight = light.add_component<components::PointLight>();
//        light.get_component<components::Transform>().set_position(glm::vec3(-1.5f, 0, -1.5f));
//        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
//
//        pointLight.set_color(vec3(0, 1, 1));
//        pointLight.set_intensity(3);
//        pointLight.set_range(10);
//
//        auto material = components::Material();
//        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
//        light.add_component<components::Material>(material);
//
//        light.add_component<components::InstanceMesh>("sphere.fbx");
//    }
//    {
//        auto light = m_scene->create_game_object("pLight2");
//        auto& pointLight = light.add_component<components::PointLight>();
//        light.get_component<components::Transform>().set_position(glm::vec3(1.5f));
//        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
//        pointLight.set_color(vec3(1, 1, 0));
//        pointLight.set_intensity(3);
//        pointLight.set_range(10);
//
//        auto material = components::Material();
//        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
//        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
//        light.add_component<components::Material>(material);
//
//        light.add_component<components::InstanceMesh>("sphere.fbx");
//    }

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
