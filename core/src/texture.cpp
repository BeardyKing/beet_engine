#include <beet/log.h>
#include <beet/texture.h>
#include <stb_image.h>

namespace beet {
namespace components {

Texture::Texture() : Asset{AssetType::Texture, ""} {}
Texture::Texture(const std::string& path) : Asset{AssetType::Texture, path} {}
Texture::~Texture() {}

void Texture::on_awake() {
    if (m_assetState != AssetState::Finished) {
        m_assetState = AssetState::Loading;
        load_texture(m_fullPath);
    }

    if (m_assetState == AssetState::Failed) {
        generate_default_asset();
    }
}

void Texture::on_destroy() {
    m_assetState = AssetState::Idle;
    glDeleteTextures(1, &m_textureHandle);
    log::info("removed texture : {}", m_fullPath);
}

void Texture::load_texture(const std::string& path) {
    auto ext = std::filesystem::path(path).extension().string();

    if (std::count(m_supported_texture_2d.begin(), m_supported_texture_2d.end(), ext)) {
        load_texture_2d(path);
    } else if (std::count(m_supported_skybox.begin(), m_supported_skybox.end(), ext)) {
        log::debug("loading HDR texture");
        load_texture_hdri(path);
    } else {
        log::error("{} on file {} is not a supported texture format", ext, path);
        m_assetState = AssetState::Failed;
    }
}

void Texture::load_texture_2d(const std::string& path, bool usingMipMaps, bool usingAnisotropicFiltering) {
    std::string fullPath = PATH_TEXTURE + path;
    std::filesystem::path fsPath = fullPath;

    log::info("Loading texture : {}", fullPath);
    if (!exists(fsPath)) {
        log::error(fullPath + " - does not Exist");
        m_textureHandle = 0;
    }

    stbi_set_flip_vertically_on_load(true);

    auto imageData = stbi_load(fullPath.c_str(), &m_imageSize.x, &m_imageSize.y, &m_channels, 0);

    if (!imageData) {
        log::error("Failed to load image: {}", fsPath.string());
        m_assetState = AssetState::Failed;
        return;
    }

    glGenTextures(1, &m_textureHandle);

    GLenum format;
    if (m_channels == 1)
        format = GL_RED;
    else if (m_channels == 3)
        format = GL_RGB;
    else if (m_channels == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_imageSize.x, m_imageSize.y, 0, format, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat maxAnisotropy{0};
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
    if (maxAnisotropy > 0 && usingAnisotropicFiltering) {
        if (m_anisotropyAmount > maxAnisotropy) {
            log::error("Setting anisotropy level to max supported level: {} - currently set to : {} ", maxAnisotropy,
                       m_anisotropyAmount);
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, m_anisotropyAmount);
    }

    if (usingMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    log::info("Texture loaded at path: {}", fullPath);

    stbi_set_flip_vertically_on_load(false);
    stbi_image_free(imageData);

    if (!m_textureHandle) {
        log::error("Error loading texture : {} - texture handle : {}", path, m_textureHandle);
        m_assetState = AssetState::Failed;
        return;
    }

    m_assetState = AssetState::Finished;
}

void Texture::load_texture_hdri(const std::string& path) {
    log::error("hdri texture loading not impl");
}

void Texture::generate_default_asset() {
    internal_generate_solid_texture(vec4(1, 0, 1, 1), "fallback texture");
}

void Texture::generate_solid_color_texture(const vec4& color, const std::string& name) {
    internal_generate_solid_texture(color, name);
}

void Texture::internal_generate_solid_texture(const vec4& color, const std::string& name) {
    m_fullPath = name;

    glGenTextures(1, &m_textureHandle);

    GLenum format;
    format = GL_RGBA;

    GLubyte texData[4];
    texData[0] = (GLuint)(color.r * 255);
    texData[1] = (GLuint)(color.g * 255);
    texData[2] = (GLuint)(color.b * 255);
    texData[3] = (GLuint)(color.a * 255);

    auto* imageData = (unsigned char*)texData;

    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, format, 1, 1, 0, format, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    log::info("generated texture: {}", m_fullPath);
    m_assetState = AssetState::Finished;
}

void Texture::bind(GLuint texUnit) {
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
}

void Texture::unbind(GLuint texUnit) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}  // namespace components
}  // namespace beet