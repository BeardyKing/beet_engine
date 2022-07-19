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

    void create_color_depth(const vec2& size, bool updatableSize = true);
    void create_color(const vec2& size, bool updatableSize = true);
    void create_depth(const vec2& size, bool updatableSize = true);
    void create_transparent(const vec2& size, bool updatableSize = true);
    void create_transparent_per_pixel_linked_list(const vec2& size, bool updatableSize = true);

    void update_size(const vec2i& size);

    GLuint get_framebuffer() { return m_fbo; };
    GLuint get_color_texture() { return m_colorTexture; };
    GLuint get_depth_texture() { return m_depthTexture; };
    GLuint get_reveal_texture() { return m_revealTexture; };

    void set_depth_to_existing_texture(GLuint depthTexture) { m_depthTexture = depthTexture; };

    void set_clear_flags(const GLbitfield flags) { m_flags = flags; };
    void clear_framebuffer();

    vec2i get_size() { return m_size; };

    void set_name(const std::string& name) { m_name = name; };
    std::string get_name() { return m_name; };
    void clear_ppll(const vec2i size);

   private:
    void create_color_attachment();
    void create_depth_attachment();
    void set_draw_buffers();

   private:
    GLuint m_fbo{0};
    GLuint m_colorTexture{0};
    GLuint m_ssboTexture{0};

    GLenum m_colorComponents = GL_RGB;
    GLenum m_colorFormat = GL_RGB;
    GLenum m_colorPixelData = GL_UNSIGNED_BYTE;

    GLuint m_depthTexture{0};
    GLuint m_revealTexture{0};

    // OIT Per Pixel Linked List
    GLuint buffers[2];
    GLuint clearBuf;

   private:
    std::vector<GLenum> m_drawBuffers;
    GLbitfield m_flags{GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT};
    vec2i m_size{1024};
    std::string m_name{"unnamed framebuffer"};
    bool m_updatableSize{true};
};

}  // namespace beet
