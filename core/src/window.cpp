// clang-format off
#include <glad/glad.h>
// clang-format on

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/window.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace beet {

Window::Window(int width, int height, std::string title, Engine& engine)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr), m_engine(engine) {
    int glfw_init_res = glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    BEET_ASSERT_MESSAGE(glfw_init_res == GLFW_TRUE, "Failed to initialize GLFW");
    log::debug("GLFW initialized");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    BEET_ASSERT_MESSAGE(m_window != nullptr, "Failed to create window");
    log::debug("Window created, title \"{}\", dimensions {}x{}", m_title, m_width, m_height);
}

void Window::setup_callbacks() {
    glfwSetWindowSizeCallback(m_window, Window::window_size_callback);
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
    Window* self = (Window*)glfwGetWindowUserPointer(window);
    self->m_width = width;
    self->m_height = height;
    self->m_engine.get_renderer_module().lock()->recreate_framebuffer(width, height);
}

bool Window::is_open() {
    return !glfwWindowShouldClose(m_window);
}

void Window::close() {
    glfwSetWindowShouldClose(m_window, true);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
    log::debug("Window destroyed");
}

void Window::on_awake() {}

void Window::on_update(double deltaTime) {
    glfwPollEvents();
    calculate_delta_time();
}

void Window::calculate_delta_time() {
    double currentFrame = glfwGetTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
}

double Window::get_delta_time() {
    return m_deltaTime;
}

void Window::on_late_update() {}
void Window::on_destroy() {}
void Window::swap_frame() {
    glfwSwapBuffers(m_window);
}

}  // namespace beet
