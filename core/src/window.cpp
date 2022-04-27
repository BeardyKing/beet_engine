#include <beet/engine.h>
#include <beet/window.h>
#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace beet {

Window::Window(int width, int height, std::string title, Engine& engine)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr), m_engine(engine) {
    int glfw_init_res = glfwInit();

    // TODO replace with spdlog / ASSERT MESSAGE
    if (glfw_init_res == GLFW_FALSE) {
        std::cout << "Failed to initialize GLFW\n";
        __debugbreak();
    }
    std::cout << "GLFW initialized\n";
    // END TODO

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    // TODO replace with spdlog / ASSERT MESSAGE
    if (m_window == nullptr) {
        std::cout << "Failed to create window\n";
        __debugbreak();
    }
    std::cout << "Window created, title " << m_title << ", dimensions : " << m_width << " , " << m_height << "\n";
    // END TODO
}

void Window::setup_callbacks() {
    glfwSetWindowSizeCallback(m_window, Window::window_size_callback);
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
    Window* self = (Window*)glfwGetWindowUserPointer(window);
    self->m_width = width;
    self->m_height = height;

    // TODO when impl call render class to recreate all framebuffer objects
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
    std::cout << "Window destroyed\n";
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

}  // namespace beet
