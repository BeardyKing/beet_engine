#include <beet/assert.h>
#include <beet/renderer.h>
#include <glad/glad.h>
namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    auto loaded = gladLoadGL();
    BEET_ASSERT_MESSAGE(loaded, "Failed to initialize GLAD");
    log::debug("GLAD initialized");
}

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
unsigned int VBO;
unsigned int VAO;

void Renderer::on_awake() {
    recreate_framebuffer(1024, 768);
    glClearColor(1.0f, 0.4f, 0.4f, 1.0f);

    m_testShader = std::make_shared<components::ShaderProgram>();
    m_testShader->set_asset_name("fallback shader");
    m_testShader->load_shader("fallback", "fallback.vert", "fallback.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

Renderer::~Renderer() {}

void Renderer::on_update(double deltaTime) {
    clear_framebuffer(0);

    glUseProgram(m_testShader->get_program());
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    depth_pass(0);
    shadow_pass(0);
    color_pass(0);
    transparent_pass(0);
    post_process_pass(0);
    gui_pass(0);
}
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

void Renderer::clear_framebuffer(uint16_t id) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::clear_all_framebuffer_objects() {
    // TODO when framebuffer manager in impl
    clear_framebuffer(0);
}

}  // namespace beet