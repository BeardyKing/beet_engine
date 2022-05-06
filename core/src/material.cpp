#include <beet/material.h>

namespace beet {
namespace components {
Material::Material() {
    m_uniformHandles.fill(0);
    m_textureHandles.fill(0);
}

void Material::on_destroy() {
    m_shader.on_destroy();
    m_uniformHandles.fill(0);
    m_textureHandles.fill(0);
}

void Material::on_awake() {
    m_shader.set_asset_name("fallback");
    m_shader.load_shader("fallback", "fallback.vert", "fallback.frag");

    auto program = m_shader.get_program();

    // clang-format off
    //TODO SETUP UBO
    m_uniformHandles[(size_t)UniformHandle::View]               = glGetUniformLocation(program, "view");
    m_uniformHandles[(size_t)UniformHandle::Projection]         = glGetUniformLocation(program, "projection");
    //TODO SETUP UBO

    m_uniformHandles[(size_t)UniformHandle::Model]              = glGetUniformLocation(program, "model");
    m_uniformHandles[(size_t)UniformHandle::AlbedoColor]        = glGetUniformLocation(program, "albedoColor");
    m_uniformHandles[(size_t)UniformHandle::TextureTiling]      = glGetUniformLocation(program, "textureTiling");
    m_uniformHandles[(size_t)UniformHandle::AlbedoScalar]       = glGetUniformLocation(program, "albedoScalar");
    m_uniformHandles[(size_t)UniformHandle::NormalScalar]       = glGetUniformLocation(program, "normalScalar");
    m_uniformHandles[(size_t)UniformHandle::MetallicScalar]     = glGetUniformLocation(program, "metallicScalar");
    m_uniformHandles[(size_t)UniformHandle::RoughnessScalar]    = glGetUniformLocation(program, "roughnessScalar");
    m_uniformHandles[(size_t)UniformHandle::OcclusionScalar]    = glGetUniformLocation(program, "occlusionScalar");
    m_uniformHandles[(size_t)UniformHandle::SkyboxScalar]       = glGetUniformLocation(program, "skyboxScalar");
    m_uniformHandles[(size_t)UniformHandle::CastShadows]        = glGetUniformLocation(program, "castShadows");
    m_uniformHandles[(size_t)UniformHandle::ReceivesShadows]    = glGetUniformLocation(program, "receivesShadows");
    m_uniformHandles[(size_t)UniformHandle::AlphaCutoffEnabled] = glGetUniformLocation(program, "alphaCutoffEnabled");
    m_uniformHandles[(size_t)UniformHandle::AlphaCutoffAmount]  = glGetUniformLocation(program, "alphaCutoffAmount");

    m_albedo    = std::make_shared<components::Texture>(m_textureSlotPath[(size_t)TextureType::Albedo]);
    m_normal    = std::make_shared<components::Texture>(m_textureSlotPath[(size_t)TextureType::Normal]);
    m_metallic  = std::make_shared<components::Texture>(m_textureSlotPath[(size_t)TextureType::Metallic]);
    m_roughness = std::make_shared<components::Texture>(m_textureSlotPath[(size_t)TextureType::Roughness]);
    m_occlusion = std::make_shared<components::Texture>(m_textureSlotPath[(size_t)TextureType::Occlusion]);

    m_albedo->on_awake();
    m_normal->on_awake();
    m_metallic->on_awake();
    m_roughness->on_awake();
    m_occlusion->on_awake();

    m_textureHandles[(size_t)TextureType::Albedo]    = m_albedo->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Normal]    = m_normal->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Metallic]  = m_metallic->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Roughness] = m_roughness->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Occlusion] = m_occlusion->get_texture_handle();

    // clang-format on
}

void Material::set_uniforms(mat4& model, mat4& view, mat4& projection) {
    // clang-format off

    // TODO SETUP UBO FOR VIEW PROJ
    glUniformMatrix4fv(m_uniformHandles[(GLuint)UniformHandle::Model],      1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_uniformHandles[(GLuint)UniformHandle::View],       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_uniformHandles[(GLuint)UniformHandle::Projection], 1, GL_FALSE, glm::value_ptr(projection));

    glUniform4f(m_uniformHandles[(size_t)UniformHandle::AlbedoColor],           m_albedoColor.r, m_albedoColor.g, m_albedoColor.b, m_albedoColor.a);
    glUniform2f(m_uniformHandles[(size_t)UniformHandle::TextureTiling],         m_textureTiling.x, m_textureTiling.y);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::AlbedoScalar],          m_albedoScalar);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::NormalScalar],          m_normalScalar);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::MetallicScalar],        m_metallicScalar);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::RoughnessScalar],       m_roughnessScalar);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::OcclusionScalar],       m_occlusionScalar);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::SkyboxScalar],          m_skyboxScalar);
    glUniform1i(m_uniformHandles[(size_t)UniformHandle::CastShadows],           m_castShadows);
    glUniform1i(m_uniformHandles[(size_t)UniformHandle::ReceivesShadows],       m_receivesShadows);
    glUniform1i(m_uniformHandles[(size_t)UniformHandle::AlphaCutoffEnabled],    m_alphaCutoffEnabled);
    glUniform1f(m_uniformHandles[(size_t)UniformHandle::AlphaCutoffAmount],     m_alphaCutoffAmount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureHandles[(size_t)TextureType::Albedo]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uniformHandles[(size_t)TextureType::Normal]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_uniformHandles[(size_t)TextureType::Metallic]);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_uniformHandles[(size_t)TextureType::Roughness]);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_uniformHandles[(size_t)TextureType::Occlusion]);

    glUseProgram(m_shader.get_program());
    // clang-format on
}

void Material::set_texture_slot_path(TextureType slot, const std::string& path) {
    if ((size_t)slot >= (size_t)TextureType::LAST) {
        log::error("invalid material texture : {}", (size_t)slot);
        return;
    }
    m_textureSlotPath[(int)slot] = path;
}

}  // namespace components
}  // namespace beet