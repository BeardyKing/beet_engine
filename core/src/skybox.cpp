#include <beet/skybox.h>

namespace beet {
namespace components {

SkyBox::SkyBox() {}

SkyBox::~SkyBox() {}

void SkyBox::on_awake() {
    m_shader.load_shader("skybox", "skybox.vert", "skybox.frag");

    m_mesh = std::make_shared<components::InstanceMesh>("default_cube.fbx");
    m_mesh->on_awake();

    auto program = m_shader.get_program();
    glUseProgram(program);

    GLuint uboMatrixIndex = glGetUniformBlockIndex(program, "Matrices");
    glUniformBlockBinding(program, uboMatrixIndex, 0);

    // clang-format off

    m_textureHandles[(size_t)SkyBoxTextureType::SkyBox]         = glGetUniformLocation(program, "albedo");
    m_textureHandles[(size_t)SkyBoxTextureType::Irradiance]     = glGetUniformLocation(program, "irradiance");
    m_textureHandles[(size_t)SkyBoxTextureType::Radiance]       = glGetUniformLocation(program, "radiance");

    m_uniformSkyboxData = glGetUniformLocation(program, "skyboxData");

    m_albedo        = AssetManager::load_asset<components::Texture>(m_textureSlotPath[(int)SkyBoxTextureType::SkyBox]).lock();
    m_irradiance    = AssetManager::load_asset<components::Texture>(m_textureSlotPath[(int)SkyBoxTextureType::Irradiance]).lock();
    m_radiance      = AssetManager::load_asset<components::Texture>(m_textureSlotPath[(int)SkyBoxTextureType::Radiance]).lock();

    // clang-format on
}
void SkyBox::set_uniforms() {
    glUseProgram(m_shader.get_program());

    m_skyboxData.x = m_exposure;
    m_skyboxData.y = (int)m_skyboxType;
    m_skyboxData.z = 0.0f;
    m_skyboxData.w = 0.0f;

    glUniform4fv(m_uniformSkyboxData, 1, value_ptr(m_skyboxData));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_albedo->get_texture_handle());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_irradiance->get_texture_handle());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_radiance->get_texture_handle());

    glUniform1i(m_textureHandles[(size_t)SkyBoxTextureType::SkyBox], 0);
    glUniform1i(m_textureHandles[(size_t)SkyBoxTextureType::Irradiance], 1);
    glUniform1i(m_textureHandles[(size_t)SkyBoxTextureType::Radiance], 2);
}

void SkyBox::set_texture_slot_path(const SkyBoxTextureType slot, const std::string& path) {
    if ((size_t)slot >= (size_t)SkyBoxTextureType::LAST) {
        log::error("invalid skybox texture : {}", (size_t)slot);
        return;
    }
    m_textureSlotPath[(int)slot] = path;
}

void SkyBox::draw() {
    m_mesh->draw();
}
void SkyBox::on_destroy() {
    m_shader.on_destroy();
    m_textureHandles.fill(0);
    m_uniformSkyboxData = 0;
}

std::shared_ptr<Texture> SkyBox::get_texture(SkyBoxTextureType slot) {
    if (slot == SkyBoxTextureType::SkyBox) {
        return m_albedo;
    }
    if (slot == SkyBoxTextureType::Irradiance) {
        return m_irradiance;
    }
    if (slot == SkyBoxTextureType::Radiance) {
        return m_radiance;
    }
}

}  // namespace components
}  // namespace beet
