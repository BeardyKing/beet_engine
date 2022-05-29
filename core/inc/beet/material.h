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
    std::unique_ptr<ShaderProgram> get_shader() { return std::make_unique<ShaderProgram>(m_shader); };

    GLuint get_texture_handle(TextureType slot);
    std::shared_ptr<Texture> get_texture(TextureType slot);
    void temp_set_shader(const std::string& folderName,
                         const std::string& vertexShaderPath,
                         const std::string& fragmentShaderPath,
                         const std::string& assetName);

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
    std::string m_tempShaderData[4] = {"lit", "lit.vert", "lit.frag", "lit"};

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

    bool m_isOpaque = true;
    float m_opacity = 0.5f;

   public:
    vec4 get_albedo_color() const { return m_albedoColor; };
    vec2 get_texture_tiling() const { return m_textureTiling; };
    float get_albedo_scalar() const { return m_albedoScalar; };
    float get_normal_scalar() const { return m_normalScalar; };
    float get_metallic_scalar() const { return m_metallicScalar; };
    float get_roughness_scalar() const { return m_roughnessScalar; };
    float get_occlusion_scalar() const { return m_occlusionScalar; };
    float get_skybox_scalar() const { return m_skyboxScalar; };
    bool get_cast_shadows() const { return m_castShadows; };
    bool get_receives_shadows() const { return m_receivesShadows; };
    bool get_alpha_cutoff_enabled() const { return m_alphaCutoffEnabled; };
    float get_alpha_cutoff_amount() const { return m_alphaCutoffAmount; };

    bool get_is_opaque() const { return m_isOpaque; };
    float get_opacity() const { return m_opacity; };

    void set_albedo_color(vec4 albedoColor) { m_albedoColor = albedoColor; };
    void set_texture_tiling(vec2 textureTiling) { m_textureTiling = textureTiling; };
    void set_albedo_scalar(float albedoScalar) { m_albedoScalar = albedoScalar; };
    void set_normal_scalar(float normalScalar) { m_normalScalar = normalScalar; };
    void set_metallic_scalar(float metallicScalar) { m_metallicScalar = metallicScalar; };
    void set_roughness_scalar(float roughnessScalar) { m_roughnessScalar = roughnessScalar; };
    void set_occlusion_scalar(float occlusionScalar) { m_occlusionScalar = occlusionScalar; };
    void set_skybox_scalar(float skyboxScalar) { m_skyboxScalar = skyboxScalar; };
    void set_cast_shadows(bool castShadows) { m_castShadows = castShadows; };
    void set_receives_shadows(bool receivesShadows) { m_receivesShadows = receivesShadows; };
    void set_alpha_cutoff_enabled(bool alphaCutoffEnabled) { m_alphaCutoffEnabled = alphaCutoffEnabled; };
    void set_alpha_cutoff_amount(float alphaCutoffAmount) { m_alphaCutoffAmount = alphaCutoffAmount; };

    void set_is_opaque(bool isOpaque) { m_isOpaque = isOpaque; };
    void set_opacity(float opacity) { m_opacity = opacity; };
};

}  // namespace components
}  // namespace beet
