#pragma once

#include <beet/log.h>
#include <beet/types.h>
#include <glad/glad.h>

namespace beet {

class Framebuffer {
   public:
    Framebuffer();
    ~Framebuffer();
    void bind();
    void unbind();

    void create_color_depth(const vec2& size);
    void create_color(const vec2& size);
    void create_depth(const vec2& size);
    void update_size(const vec2i& size);

    GLuint get_framebuffer() { return m_fbo; };
    GLuint get_color_texture() { return m_colorTexture; };
    GLuint get_depth_texture() { return m_depthTexture; };

    void set_clear_flags(const GLbitfield flags) { m_flags = flags; };
    void clear_framebuffer();

   private:
    void create_color_attachment(const vec2& size);
    void create_depth_attachment(const vec2& size);
    void set_draw_buffers();

   private:
    GLuint m_fbo{0};
    GLuint m_colorTexture{0};
    GLuint m_depthTexture{0};

   private:
    std::vector<GLenum> m_drawBuffers;
    GLbitfield m_flags{GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT};
};

}  // namespace beet
