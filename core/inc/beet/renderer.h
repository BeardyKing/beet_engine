#pragma once

#include <beet/log.h>
#include <beet/subsystem.h>

namespace beet {
class Engine;
}  // namespace beet

namespace beet {

class Renderer : public Subsystem {
   public:
    explicit Renderer(Engine& engine);
    ~Renderer();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void recreate_framebuffer(uint16_t width, uint16_t height, uint16_t id = 0);
    void clear_framebuffer(uint16_t id = 0);

   private:
    // 1) SYSTEM : Shadow pass
    // 2) SYSTEM : Update Active Camera (Runtime / Editor)
    // 3) SYSTEM : Depth Pass (No Transparent Objects)
    // 4) SYSTEM : Color Render Pass
    // 5) SYSTEM : Skybox Render
    // 6) SYSTEM : Sorted Transparent Render Pass
    // 7) SYSTEM : Post Processing Stack

    void shadow_pass(uint16_t id);
    void depth_pass(uint16_t id);
    void color_pass(uint16_t id);
    void gui_pass(uint16_t id);
    void transparent_pass(uint16_t id);
    void post_process_pass(uint16_t id);

   private:
    int get_window_width();
    int get_window_height();

    Engine& m_engine;
};

}  // namespace beet
