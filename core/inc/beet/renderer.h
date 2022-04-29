#pragma once

#include <beet/log.h>
#include <beet/mesh.h>
#include <beet/shader_program.h>
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
    void clear_framebuffer(uint16_t id = 0);
    void clear_all_framebuffer_objects();

    Engine& m_engine;

    std::shared_ptr<components::ShaderProgram> m_testShader;
    std::shared_ptr<components::Mesh> m_testMesh;
    int m_modelLoc{0};
    int m_viewLoc{0};
    int m_projLoc{0};
    vec4 m_clearCol{1.0f, 0.4f, 0.4f, 1.0f};
    float m_timePassed{0.0f};
};

}  // namespace beet
