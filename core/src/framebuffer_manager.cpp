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
    m_framebuffers[(size_t)FrameBufferType::Color] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::PostProcess] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::Gui] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::ShadowOne] = Framebuffer();
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo] = Framebuffer();

    // TODO IMPL depth & color only
    m_framebuffers[(size_t)FrameBufferType::Back];  // fbo default = 0;
    m_framebuffers[(size_t)FrameBufferType::Depth].create_color_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::Color].create_color_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::PostProcess].create_color_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::Gui].create_color_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::ShadowOne].create_color_depth(windowSize);
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo].create_color_depth(windowSize);

    m_framebuffers[(size_t)FrameBufferType::Back].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::Depth].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::Color].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::PostProcess].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::Gui].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::ShadowOne].set_clear_flags(clearFlagsColorDepth);
    m_framebuffers[(size_t)FrameBufferType::ShadowTwo].set_clear_flags(clearFlagsColorDepth);
}

void FramebufferManager::on_update(double m_delta_time) {}

void FramebufferManager::on_late_update() {}

void FramebufferManager::on_destroy() {
    log::info("FramebufferManager destroyed");
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

}  // namespace beet