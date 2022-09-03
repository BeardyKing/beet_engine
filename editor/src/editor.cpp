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
        auto cubeObj = m_scene->create_game_object("Weighted Blended - stanford dragon");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, -1.5, 0));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 90, 0));

        cubeObj.add_component<components::InstanceMesh>("dragon.fbx");

        auto material = components::Material();
        material.temp_set_shader("oit_wb", "lit_gather_wb.vert", "lit_gather_wb.frag", "lit_gather_wb");
        material.set_albedo_scalar(0.968f);
        material.set_normal_scalar(1.86);
        material.set_metallic_scalar(1.0f);
        material.set_roughness_scalar(2.73f);
        material.set_occlusion_scalar(1.06f);

        material.set_is_opaque(false);
        material.set_albedo_color(vec4(250.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 185.0f / 255.0f));
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
        light.get_component<components::Transform>().set_position(glm::vec3(1.089, -0.535, 0.578));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(0, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
    }
    {
        auto light = m_scene->create_game_object("pLight1");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(-0.841, 0.039, -0.924));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));

        pointLight.set_color(vec3(0, 1, 1));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
    }
    {
        auto light = m_scene->create_game_object("pLight2");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(1.01, 0.084, -0.869));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(1, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
    }
    {
        auto light = m_scene->create_game_object("pLight3");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(-0.577, -1.139, 1.294));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(1, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);
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
