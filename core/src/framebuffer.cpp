#include <beet/framebuffer.h>
namespace beet {

Framebuffer::Framebuffer() {}

Framebuffer::~Framebuffer() {}

void Framebuffer::create_color_depth(const vec2& size) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_color_attachment(size);
    create_depth_attachment(size);

    set_draw_buffers();
}

void Framebuffer::create_color(const vec2& size) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_color_attachment(size);

    set_draw_buffers();
}

void Framebuffer::create_depth(const vec2& size) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_depth_attachment(size);

    set_draw_buffers();
}

void Framebuffer::set_draw_buffers() {
    glBindBuffer(GL_FRAMEBUFFER, m_fbo);
    glDrawBuffers(m_drawBuffers.size(), m_drawBuffers.data());
    glBindBuffer(GL_FRAMEBUFFER, 0);
    log::info("Created framebuffer {} - with [{}] attachments", m_fbo, m_drawBuffers.size());
}

void Framebuffer::create_depth_attachment(const vec2& size) {
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
    m_drawBuffers.emplace_back(GL_DEPTH_ATTACHMENT);
}

void Framebuffer::create_color_attachment(const vec2& size) {
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorTexture, 0);
    m_drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::update_size(const vec2i& size) {
    if (m_colorTexture) {
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    if (m_depthTexture) {
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }
}

void Framebuffer::clear_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(m_flags);
}

}  // namespace beet