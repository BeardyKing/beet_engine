#include <beet/asset_manager.h>
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

    GLuint uboMatrixIndex = glGetUniformBlockIndex(program, "Matrices");
    glUniformBlockBinding(program, uboMatrixIndex, 0);

    GLuint uboPointLightIndex = glGetUniformBlockIndex(program, "PointLights");
    glUniformBlockBinding(program, uboPointLightIndex, 1);

    m_uniformHandles[(size_t)UniformHandle::Model] = glGetUniformLocation(program, "model");
    m_uniformHandles[(size_t)UniformHandle::AlbedoColor] = glGetUniformLocation(program, "albedoColor");
    m_uniformHandles[(size_t)UniformHandle::TextureTiling] = glGetUniformLocation(program, "textureTiling");
    m_uniformHandles[(size_t)UniformHandle::AlbedoScalar] = glGetUniformLocation(program, "albedoScalar");
    m_uniformHandles[(size_t)UniformHandle::NormalScalar] = glGetUniformLocation(program, "normalScalar");
    m_uniformHandles[(size_t)UniformHandle::MetallicScalar] = glGetUniformLocation(program, "metallicScalar");
    m_uniformHandles[(size_t)UniformHandle::RoughnessScalar] = glGetUniformLocation(program, "roughnessScalar");
    m_uniformHandles[(size_t)UniformHandle::OcclusionScalar] = glGetUniformLocation(program, "occlusionScalar");
    m_uniformHandles[(size_t)UniformHandle::SkyboxScalar] = glGetUniformLocation(program, "skyboxScalar");
    m_uniformHandles[(size_t)UniformHandle::CastShadows] = glGetUniformLocation(program, "castShadows");
    m_uniformHandles[(size_t)UniformHandle::ReceivesShadows] = glGetUniformLocation(program, "receivesShadows");
    m_uniformHandles[(size_t)UniformHandle::AlphaCutoffEnabled] = glGetUniformLocation(program, "alphaCutoffEnabled");
    m_uniformHandles[(size_t)UniformHandle::AlphaCutoffAmount] = glGetUniformLocation(program, "alphaCutoffAmount");

    m_albedo = AssetManager::load_asset<components::Texture>(m_textureSlotPath[(size_t)TextureType::Albedo]).lock();
    m_normal = AssetManager::load_asset<components::Texture>(m_textureSlotPath[(size_t)TextureType::Normal]).lock();
    m_metallic = AssetManager::load_asset<components::Texture>(m_textureSlotPath[(size_t)TextureType::Metallic]).lock();
    m_roughness =
        AssetManager::load_asset<components::Texture>(m_textureSlotPath[(size_t)TextureType::Roughness]).lock();
    m_occlusion =
        AssetManager::load_asset<components::Texture>(m_textureSlotPath[(size_t)TextureType::Occlusion]).lock();

    m_albedo->on_awake();
    m_normal->on_awake();
    m_metallic->on_awake();
    m_roughness->on_awake();
    m_occlusion->on_awake();

    m_textureHandles[(size_t)TextureType::Albedo] = m_albedo->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Normal] = m_normal->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Metallic] = m_metallic->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Roughness] = m_roughness->get_texture_handle();
    m_textureHandles[(size_t)TextureType::Occlusion] = m_occlusion->get_texture_handle();

    // clang-format on
}

void Material::set_uniforms(mat4& model, mat4& view, mat4& projection) {
    // clang-format off

    glUniformMatrix4fv(m_uniformHandles[(GLuint)UniformHandle::Model],          1, GL_FALSE, value_ptr(model));

    glUniform4fv(m_uniformHandles[(size_t)UniformHandle::AlbedoColor],          1, value_ptr(m_albedoColor));
    glUniform2fv(m_uniformHandles[(size_t)UniformHandle::TextureTiling],        1, value_ptr(m_textureTiling));

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