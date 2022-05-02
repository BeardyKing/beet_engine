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
    : m_width(width),
      m_height(height),
      m_title(title),
      m_window(nullptr),
      m_engine(engine),
      m_input(std::make_shared<InputManager>(*this)) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    BEET_ASSERT_MESSAGE(m_window, "Failed to create GLFW window");
    log::debug("GLFW initialized");

    glfwMakeContextCurrent(m_window);
}

void Window::on_awake() {
    glfwSetWindowUserPointer(m_window, this);
    setup_callbacks();
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
    auto* self = (Window*)glfwGetWindowUserPointer(window);
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

void Window::setup_callbacks() {
    glfwSetWindowSizeCallback(m_window, Window::window_size_callback);

    glfwSetKeyCallback(m_window, Window::window_key_callback);
    glfwSetCharCallback(m_window, Window::window_char_callback);
    glfwSetMouseButtonCallback(m_window, Window::window_mouse_button_callback);
    glfwSetScrollCallback(m_window, Window::window_scroll_event);
    glfwSetCursorPosCallback(m_window, Window::window_mouse_event_callback);
}

void Window::window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (action != GLFW_REPEAT) {
        self->m_input->key_event(key, action != GLFW_RELEASE);
    }
}

void Window::window_char_callback(GLFWwindow* window, unsigned int codepoint) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
}

void Window::window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_input->mouse_button_event(button, action != GLFW_RELEASE);
}

void Window::window_scroll_event(GLFWwindow* window, double xoffset, double yoffset) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_input->scroll_event({xoffset, yoffset});
}

void Window::window_mouse_event_callback(GLFWwindow* window, double x, double y) {
    if (!glfwGetWindowAttrib(window, GLFW_HOVERED))
        return;
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_input->mouse_event(x, y);
}

void Window::window_cursor_enter_event_callback(GLFWwindow* window, int entered) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
}
std::shared_ptr<InputManager> Window::get_input_manager() {
    return m_input;
}

}  // namespace beet
