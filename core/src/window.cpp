
// clang-format off
#include <glad/glad.h>
// clang-format on

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/window.h>

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
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    BEET_ASSERT_MESSAGE(m_window, "Failed to create GLFW window");
    log::debug("GLFW initialized");

    glfwMakeContextCurrent(m_window);
}

void Window::on_awake() {
    glfwSetWindowUserPointer(m_window, this);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSwapInterval(1);
    setup_callbacks();
    m_engine.get_renderer_module().lock()->resize_all_framebuffers(vec2i(m_width, m_height));
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
    auto* self = (Window*)glfwGetWindowUserPointer(window);
    vec2i size{width, height};
    self->m_width = size.x;
    self->m_height = size.y;
    self->m_engine.get_renderer_module().lock()->resize_all_framebuffers(size);
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
    toggle_fullscreen();
}
void Window::toggle_fullscreen() {
    if (m_input->key_on_trigger(KeyCode::LeftAlt) && m_input->key_on_trigger(KeyCode::Enter) && !keysDown) {
        toggle_fullscreen_internal();
        keysDown = true;
    } else if (!m_input->key_pressed(KeyCode::LeftAlt) || !m_input->key_pressed(KeyCode::Enter)) {
        keysDown = false;
    }
}

void Window::toggle_fullscreen_internal() {
    vec2i windowPos;
    glfwGetWindowPos(m_window, &windowPos.x, &windowPos.y);
    m_fullscreen = !m_fullscreen;
    if (m_fullscreen) {
        // best window selection from https://stackoverflow.com/a/31526753
        int nmonitors, i;
        int wx, wy, ww, wh;
        int mx, my, mw, mh;
        int overlap, bestoverlap;
        GLFWmonitor* bestmonitor;
        GLFWmonitor** monitors;
        const GLFWvidmode* mode;

        bestoverlap = 0;
        bestmonitor = NULL;

        glfwGetWindowPos(m_window, &wx, &wy);
        glfwGetWindowSize(m_window, &ww, &wh);
        monitors = glfwGetMonitors(&nmonitors);

        for (i = 0; i < nmonitors; i++) {
            mode = glfwGetVideoMode(monitors[i]);
            glfwGetMonitorPos(monitors[i], &mx, &my);
            mw = mode->width;
            mh = mode->height;

            overlap = max(0, min(wx + ww, mx + mw) - max(wx, mx)) * max(0, min(wy + wh, my + mh) - max(wy, my));

            if (bestoverlap < overlap) {
                bestoverlap = overlap;
                bestmonitor = monitors[i];
            }
        }
        auto bestMode = glfwGetVideoMode(bestmonitor);

        m_width = bestMode->width;
        m_height = bestMode->height;

        glfwSetWindowMonitor(m_window, bestmonitor, windowPos.x, 0, bestMode->width, bestMode->height,
                             bestMode->refreshRate);
    } else if (!m_fullscreen) {
        glfwSetWindowMonitor(m_window, NULL, windowPos.x, windowPos.y + WINDOWS_TITLE_BAR_SIZE, m_width, m_height, 0);
    }

    log::debug("Window toggle size : {}, {}", m_width, m_height);
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

void Window::set_cursor_hide(bool state) {
    glfwSetInputMode(m_window, GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

std::shared_ptr<InputManager> Window::get_input_manager() {
    return m_input;
}

}  // namespace beet
