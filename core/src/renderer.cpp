#include <beet/assert.h>
#include <beet/renderer.h>
#include <glad/glad.h>
namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    BEET_ASSERT_MESSAGE(gladLoadGL() != GL_TRUE, "Failed to initialize GLAD");
    log::debug("GLAD initialized");
}

Renderer::~Renderer() {}

void Renderer::on_awake() {}

void Renderer::on_update(double deltaTime) {}
void Renderer::on_late_update() {}
void Renderer::on_destroy() {}

void Renderer::shadow_pass(uint16_t id) {}
void Renderer::depth_pass(uint16_t id) {}
void Renderer::color_pass(uint16_t id) {}
void Renderer::gui_pass(uint16_t id) {}
void Renderer::transparent_pass(uint16_t id) {}
void Renderer::post_process_pass(uint16_t id) {}

void Renderer::recreate_framebuffer(uint16_t width, uint16_t height, uint16_t id) {
    glViewport(0, 0, width, height);
}
void Renderer::clear_framebuffer(uint16_t id) {}

}  // namespace beet
