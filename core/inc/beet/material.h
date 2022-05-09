#pragma once

#include <beet/component.h>
#include <beet/log.h>
#include <beet/shader_program.h>
#include <beet/texture.h>
#include <beet/types.h>
#include <array>

namespace beet {
namespace components {
// clang-format off

enum class UniformHandle {
    Model,
    AlbedoColor,
    TextureTiling,
    AlbedoScalar,
    NormalScalar,
    MetallicScalar,
    RoughnessScalar,
    OcclusionScalar,
    SkyboxScalar,
    CastShadows,
    ReceivesShadows,
    AlphaCutoffEnabled,
    AlphaCutoffAmount,
    LAST
};

// clang-format on

class Material : public Component<Material> {
   public:
    Material();
    Material(std::array<std::string, 5> textures);
    ~Material() = default;

    //=For ECS========
    void on_awake() override;
    void on_destroy() override;
    //================

    void set_texture_slot_path(TextureType slot, const std::string& path);

    void set_uniforms(const mat4& model);
    GLuint get_program() { return m_shader.get_program(); };

   private:
    std::array<GLint, (size_t)UniformHandle::LAST> m_uniformHandles;
    std::array<GLint, (size_t)TextureType::LAST> m_textureHandles;

   private:
    std::shared_ptr<Texture> m_albedo;
    std::shared_ptr<Texture> m_normal;
    std::shared_ptr<Texture> m_metallic;
    std::shared_ptr<Texture> m_roughness;
    std::shared_ptr<Texture> m_occlusion;

    std::string m_textureSlotPath[5] = {"", "", "", "", ""};

    ShaderProgram m_shader;

   private:
    glm::vec4 m_albedoColor = glm::vec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
    glm::vec2 m_textureTiling = glm::vec2(1);

    float m_albedoScalar = 1.0f;
    float m_normalScalar = 1.0f;
    float m_metallicScalar = 1.0f;
    float m_roughnessScalar = 1.0f;
    float m_occlusionScalar = 1.0f;
    float m_skyboxScalar = 0.2f;

    bool m_castShadows = true;
    bool m_receivesShadows = true;

    bool m_alphaCutoffEnabled = false;
    float m_alphaCutoffAmount = 0.0f;
};

}  // namespace components
}  // namespace beet
