#pragma once

#include <beet/subsystem.h>
#include <string>

namespace beet {
class Engine;
}
class GLFWwindow;

namespace beet {

class Window : public Subsystem {
   public:
    Window(int width, int height, std::string title, Engine& engine);
    ~Window();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void swap_frame();
    bool is_open();
    void close();

    void calculate_delta_time();
    double get_delta_time();

    int get_window_width() { return m_width; };
    int get_window_height() { return m_height; };
    float get_window_aspect_ratio() { return (float)m_width / (float)m_height; }

   protected:
    void setup_callbacks();
    static void window_size_callback(GLFWwindow* window, int width, int height);

   protected:
    int m_width;
    int m_height;
    std::string m_title;

    double m_deltaTime = 0;
    double m_lastFrame = 0;

    static constexpr double PHYSICS_TIMESTEP = 0.2;
    double m_fixedDeltaTime = PHYSICS_TIMESTEP;

    GLFWwindow* m_window;
    Engine& m_engine;
};

}  // namespace beet
