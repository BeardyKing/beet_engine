#pragma once
#include <beet/framebuffer.h>
#include <beet/subsystem.h>
#include <beet/types.h>
#include <glad/glad.h>
#include <memory>

namespace beet {

class Engine;

}

namespace beet {

class FramebufferManager : public Subsystem {
   public:
    explicit FramebufferManager(Engine& engine);
    ~FramebufferManager();

    void on_awake() override;
    void on_update(double m_delta_time) override;
    void on_late_update() override;
    void on_destroy() override;

    void resize_all_framebuffers(const vec2i& size);
    void clear_all_framebuffers();

    void bind_framebuffer(FrameBufferType type);
    void unbind_framebuffer();

    GLuint get_color_attachment(FrameBufferType type);

   private:
    std::array<Framebuffer, (size_t)FrameBufferType::LAST> m_framebuffers;
    Engine& m_engine;
};

}  // namespace beet
