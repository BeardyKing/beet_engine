#pragma once

#include <glad/glad.h>

#include <beet/asset_manager.h>
#include <beet/component.h>
#include <beet/instance_mesh.h>
#include <beet/shader_program.h>
#include <beet/texture.h>
#include <beet/types.h>
#include <memory>

namespace beet {
namespace components {

class SkyBox : public Component<SkyBox> {
   public:
    SkyBox();
    ~SkyBox();

    void on_awake() override;
    void on_destroy() override;

   public:
    void set_texture_slot_path(const SkyBoxTextureType slot, const std::string& path);

    void set_uniforms(const mat4& invProj);
    GLint get_program() { return m_shader.get_program(); };
    void draw();
    void set_uniform_exposure(float exposure) { m_exposure = exposure; };
    float get_uniform_exposure() { return m_exposure; };

    void set_uniform_background_type(SkyBoxTextureType type) { m_skyboxType = type; };
    SkyBoxTextureType get_uniform_background_type() { return m_skyboxType; };

   private:
    std::array<GLint, (size_t)SkyBoxTextureType::LAST> m_textureHandles;

   private:
    std::shared_ptr<Texture> m_albedo;
    std::shared_ptr<Texture> m_irradiance;
    std::shared_ptr<Texture> m_radiance;

    std::string m_textureSlotPath[3] = {"", "", ""};
    GLint m_uniformSkyboxData;
    GLint m_invProj;
    vec4 m_skyboxData;

    float m_exposure = 1.0f;
    SkyBoxTextureType m_skyboxType = SkyBoxTextureType::Radiance;

    ShaderProgram m_shader;
    std::shared_ptr<InstanceMesh> m_mesh;
};
}  // namespace components
}  // namespace beet
