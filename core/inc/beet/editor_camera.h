#pragma once

#include <beet/subsystem.h>
#include <beet/types.h>
#include <memory>

namespace beet {

class Engine;
class InputManager;

}  // namespace beet

namespace beet {

class EditorCameraController : public Subsystem {
   public:
    EditorCameraController(Engine& engine);

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    vec3 get_rotation_euler() { return {m_pitch, m_yaw, m_roll}; }
    void set_rotation_euler(vec3 rotation) {
        m_pitch = rotation.x;
        m_yaw = rotation.y;
        m_roll = rotation.z;
    }

   private:
    void camera_key_input();

   private:
    Engine& m_engine;
    std::shared_ptr<InputManager> m_inputManager;

    vec2d m_lastMousePosition = vec2d(0.0);
    vec2d m_mouseSensitivity = vec2d(0.2);
    vec2d m_pitchClamp = vec2(-60, 60);
    vec2d m_currentMousePos = vec2d(0.0);
    vec2d m_mouseDelta = vec2d(0.0);

    vec3 m_movementDirection = vec3(0);
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

    double m_timePassed = 0;

    bool m_viewToggleFlag = false;
    // TODO after perf testing reset these to false
    bool m_lockState = true;
    bool m_autoOrbit = true;

    float m_orbitDistance = 3.0f;
    float m_orbitSpeed = 0.25f;
    vec3 m_orbitLook = vec3(0.0f);
};

}  // namespace beet
