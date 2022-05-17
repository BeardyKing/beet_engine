#include <beet/framebuffer.h>
namespace beet {

Framebuffer::Framebuffer() {}

Framebuffer::~Framebuffer() {}

void Framebuffer::create_color_depth(const vec2& size) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    attach_color(size);
    attach_depth(size);

    glDrawBuffers(m_drawBuffers.size(), m_drawBuffers.data());
    glBindBuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attach_depth(const vec2& size) {
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
    m_drawBuffers.emplace_back(GL_DEPTH_ATTACHMENT);
}

void Framebuffer::attach_color(const vec2& size) {
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
    glEnable(GL_DEPTH_TEST);  // enable depth testing (is disabled for rendering screen-space quad)

    glClear(m_flags);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::update_size(const vec2& size) {
    glBindTexture(GL_TEXTURE_2D, m_fbo);

    if (m_colorTexture) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    if (m_depthTexture) {
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    glViewport(0, 0, size.x, size.y);  // TODO CHECK IF THIS IS NEEDED
}

}  // namespace beet