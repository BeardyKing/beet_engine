#include <beet/engine.h>
#include <beet/framebuffer_manager.h>

namespace beet {

FramebufferManager::FramebufferManager(Engine& engine) : m_engine(engine) {}

void FramebufferManager::on_awake() {
    auto windowSize = m_engine.get_window_module().lock()->get_window_size();
    auto clearFlagsColorDepth = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    auto clearFlagsColor = GL_COLOR_BUFFER_BIT;
    auto clearFlagsDepth = GL_DEPTH_BUFFER_BIT;

    m_framebuffers[(size_t)FrameBufferType::Back] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::Depth] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::Opaque] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::Transparency] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::PostProcess] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::ObjectPicking] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::ShadowOne] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo] = Framebuffer();

    m_framebuffers[(size_t)FrameBufferType::Back];  // fbo default = 0;
    m_framebuffers[(size_t)FrameBufferType::Depth].create_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::Opaque].create_color_depth(windowSize);

    m_framebuffers[(size_t)FrameBufferType::Transparency].set_depth_to_existing_texture(
        m_framebuffers[(size_t)FrameBufferType::Opaque].get_depth_texture());
    m_framebuffers[(size_t)FrameBufferType::Transparency].create_transparent(windowSize);

    m_framebuffers[(size_t)FrameBufferType::PostProcess].create_color(windowSize);
    m_framebuffers[(size_t)FrameBufferType::ObjectPicking].create_color_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::ShadowOne].create_depth(vec2i(1024), false);
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo].create_depth(vec2i(1024), false);

    m_framebuffers[(size_t)FrameBufferType::Back].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::Depth].set_clear_flags(clearFlagsDepth);
    m_framebuffers[(size_t)FrameBufferType::Opaque].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::Transparency].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::PostProcess].set_clear_flags(clearFlagsColor);
    m_framebuffers[(size_t)FrameBufferType::ObjectPicking].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::ShadowOne].set_clear_flags(clearFlagsDepth);
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo].set_clear_flags(clearFlagsDepth);

    m_framebuffers[(size_t)FrameBufferType::Back].set_name("Back buffer");
    m_framebuffers[(size_t)FrameBufferType::Depth].set_name("Depth pre pass");
    m_framebuffers[(size_t)FrameBufferType::Opaque].set_name("Opaque");
    m_framebuffers[(size_t)FrameBufferType::Transparency].set_name("Transparency");
    m_framebuffers[(size_t)FrameBufferType::PostProcess].set_name("Post process");
    m_framebuffers[(size_t)FrameBufferType::ObjectPicking].set_name("Picking pre pass");
    m_framebuffers[(size_t)FrameBufferType::ShadowOne].set_name("Shadow one");
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo].set_name("Shadow two");
}

void FramebufferManager::on_update(double m_delta_time) {}

void FramebufferManager::on_late_update() {}

void FramebufferManager::on_destroy() {
    log::debug("FramebufferManager destroyed");
}

FramebufferManager::~FramebufferManager() {}

void FramebufferManager::resize_all_framebuffers(const vec2i& size) {
    for (auto& fb : m_framebuffers) {
        fb.update_size(size);
    }
}

void FramebufferManager::clear_all_framebuffers() {
    for (auto& fb : m_framebuffers) {
        fb.clear_framebuffer();
    }
}

void FramebufferManager::bind_framebuffer(FrameBufferType type) {
    m_framebuffers[(size_t)type].bind();
}

void FramebufferManager::unbind_framebuffer() {
    m_framebuffers[(size_t)FrameBufferType::Back].bind();
}

GLuint FramebufferManager::get_color_attachment(FrameBufferType type) {
    return m_framebuffers[(size_t)type].get_color_texture();
}

GLuint FramebufferManager::get_reveal_attachment(FrameBufferType type) {
    return m_framebuffers[(size_t)type].get_reveal_texture();
}

}  // namespace beet