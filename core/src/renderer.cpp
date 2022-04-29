
#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/types.h>
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
    glClearColor(m_clearCol.x, m_clearCol.y, m_clearCol.z, m_clearCol.w);

    m_testMesh = std::make_shared<components::Mesh>("uv_cube.obj");
    m_testMesh->on_awake();

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

    //=setup uniforms

    m_modelLoc = glGetUniformLocation(m_testShader->get_program(), "model");
    m_viewLoc = glGetUniformLocation(m_testShader->get_program(), "view");
    m_projLoc = glGetUniformLocation(m_testShader->get_program(), "projection");
}

Renderer::~Renderer() {}

void Renderer::on_update(double deltaTime) {
    clear_framebuffer(0);

    float aspectRatio = (float)m_engine.get_window_module().lock()->get_window_aspect_ratio();
    m_timePassed += (float)deltaTime;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(m_timePassed * 50), glm::vec3(1.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    //=RENDER=======================
    glUseProgram(m_testShader->get_program());
    m_testMesh->draw();

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
