#pragma once

#include <beet/camera.h>
#include <beet/log.h>
#include <beet/mesh.h>
#include <beet/shader_program.h>
#include <beet/subsystem.h>
#include <beet/texture.h>
#include <beet/transform.h>

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
    std::shared_ptr<components::Texture> m_testTexture;

    //=Camera=========
    std::shared_ptr<components::Camera> m_testCamera;
    std::shared_ptr<components::Transform> m_testCameraTransform;

    //=Camera state===

    vec2d m_lastMousePosition = vec2d(0.0);
    vec2d m_mouseSensitivity = vec2d(0.2);
    vec2d m_pitchClamp = vec2(-60, 60);
    vec2d m_currentMousePos = vec2d(0.0);
    vec2d m_mouseDelta = vec2d(0.0);

    vec3 m_keyboardDirection = vec3(0);
    vec3 m_movementMultiplier = vec3(1);
    vec3 m_maxMovementMultiplier = vec3(5);
    vec3 m_minMovementMultiplier = vec3(1);
    vec3 m_moveSpeed = vec3(10);

    float m_pitch = 0;  // pitch = x
    float m_yaw = 0;    // yaw = y
    float m_roll = 0;   // roll = z

    vec3 m_right = vec3(1, 0, 0);
    vec3 m_up = vec3(0, 1, 0);
    vec3 m_forward = vec3(0, 0, 1);
    static constexpr vec3 WORLD_UP = vec3(0, 1, 0);

    bool m_ePressed = true;
    bool m_lockState = false;
    //================

    int m_modelLoc{0};
    int m_viewLoc{0};
    int m_projLoc{0};
    vec4 m_clearCol{1.0f, 0.4f, 0.4f, 1.0f};
    float m_timePassed{0.0f};
};

}  // namespace beet
