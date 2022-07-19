#include <beet/framebuffer.h>
namespace beet {

Framebuffer::Framebuffer() {}

Framebuffer::~Framebuffer() {
    if (m_fbo) {
        glDeleteBuffers(1, &m_fbo);
    }
}

void Framebuffer::create_color_depth(const vec2& size, bool updatableSize) {
    m_size = size;
    m_updatableSize = updatableSize;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_color_attachment();
    create_depth_attachment();

    set_draw_buffers();
}

void Framebuffer::create_color(const vec2& size, bool updatableSize) {
    m_size = size;
    m_updatableSize = updatableSize;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_color_attachment();

    set_draw_buffers();
}

void Framebuffer::create_depth(const vec2& size, bool updatableSize) {
    m_size = size;
    m_updatableSize = updatableSize;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_depth_attachment();

    set_draw_buffers();
}

enum BufferNames { COUNTER_BUFFER = 0, LINKED_LIST_BUFFER };

void Framebuffer::create_transparent_per_pixel_linked_list(const vec2& size, bool updatableSize) {
    m_size = size;
    m_updatableSize = updatableSize;

    m_colorComponents = GL_R32UI;

    glGenTextures(1, &m_fbo);
    glBindTexture(GL_TEXTURE_2D, m_ssboTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, size.x, size.y);
    glBindImageTexture(0, m_fbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    glGenBuffers(2, buffers);
    GLuint maxNodes = 20 * m_size.x * m_size.y;
    GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);  // The size of a linked list node

    // Our atomic counter
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER]);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

    // The buffer for the head pointers, as an image texture
    glGenTextures(1, &m_fbo);
    glBindTexture(GL_TEXTURE_2D, m_fbo);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, size.x, size.y);
    glBindImageTexture(0, m_fbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    // The buffer of linked lists
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[LINKED_LIST_BUFFER]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);

    //    prog.setUniform("MaxNodes", maxNodes);

    std::vector<GLuint> headPtrClearBuf(m_size.x * m_size.y, 0xffffffff);
    glGenBuffers(1, &clearBuf);
    //    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
    //    glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint), &headPtrClearBuf[0],
    //    GL_STATIC_COPY); glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Framebuffer::clear_ppll(const vec2i size) {
    //    GLuint zero = 0;
    //    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER]);
    //    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
    //
    //    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
    //    glBindTexture(GL_TEXTURE_2D, m_fbo);
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
}

void Framebuffer::create_transparent(const vec2& size, bool updatableSize) {
    m_size = size;
    m_updatableSize = updatableSize;

    m_colorComponents = GL_RGBA16F;
    m_colorFormat = GL_RGBA;
    m_colorPixelData = GL_HALF_FLOAT;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, m_colorComponents, m_size.x, m_size.y, 0, m_colorFormat, m_colorPixelData, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_revealTexture);
    glBindTexture(GL_TEXTURE_2D, m_revealTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_size.x, m_size.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (m_depthTexture == 0) {
        glGenTextures(1, &m_depthTexture);
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_size.x, m_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
    } else {
        log::info("using existing depth texture {} - on fbo : {}", m_depthTexture, m_fbo);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_revealTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    // opaque framebuffer's depth texture

    const GLenum transparentDrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, transparentDrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        log::error("ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::set_draw_buffers() {
    glBindBuffer(GL_FRAMEBUFFER, m_fbo);
    glDrawBuffers(m_drawBuffers.size(), m_drawBuffers.data());
    glBindBuffer(GL_FRAMEBUFFER, 0);
    log::info("Created framebuffer {} - with [{}] attachments, of size [{},{}]", m_fbo, m_drawBuffers.size(), m_size.x,
              m_size.y);
}

void Framebuffer::create_depth_attachment() {
    if (m_depthTexture == 0) {
        glGenTextures(1, &m_depthTexture);
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_size.x, m_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
    } else {
        log::info("using existing depth texture {} - on fbo : {}", m_depthTexture, m_fbo);
    }
}

void Framebuffer::create_color_attachment() {
    m_colorComponents = GL_RGB16;
    m_colorFormat = GL_RGB;
    m_colorPixelData = GL_UNSIGNED_BYTE;

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, m_colorComponents, m_size.x, m_size.y, 0, m_colorFormat, m_colorPixelData, 0);

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
    if (m_updatableSize == false) {
        return;
    }

    m_size = size;

    // TODO component vector of texture attachment class that manages storing
    // attachment data for easy editor modifying and size updating
    // (ie this is going to become hard to maintain unless can be managed via editor)

    if (m_colorTexture) {
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, m_colorComponents, m_size.x, m_size.y, 0, m_colorFormat, m_colorPixelData, 0);
    }

    if (m_depthTexture) {
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_size.x, m_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    if (m_revealTexture) {
        glBindTexture(GL_TEXTURE_2D, m_revealTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_size.x, m_size.y, 0, GL_RED, GL_FLOAT, 0);
    }
}

void Framebuffer::clear_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(m_flags);
}

}  // namespace beet