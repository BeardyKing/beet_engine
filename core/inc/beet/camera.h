#pragma once

#include <beet/component.h>
#include <beet/types.h>
#include <optional>

namespace beet {
namespace components {

class Camera : public Component<Camera> {
   public:
    Camera();
    ~Camera();

    void on_awake() override;
    void on_destroy() override;

    vec3 get_look_target() { return m_lookTarget; };
    float get_fov() { return radians(m_fov); };
    float get_z_near() { return m_zNear; };
    float get_z_far() { return m_zFar; };
    float get_move_speed() { return m_moveSpeed; };

    vec3 world_up();

    void set_look_target(vec3 target) { m_lookTarget = target; };
    void set_fov(float fov) { m_fov = fov; };
    void set_z_near(float zNear) { m_zNear = zNear; };
    void set_z_far(float zFar) { m_zFar = zFar; };
    void set_move_speed(float speed) { m_moveSpeed = speed; };

    static std::optional<std::reference_wrapper<Camera>> get_active_camera() { return s_activeCamera; }

    static void set_active_camera(std::optional<std::reference_wrapper<Camera>> camera) { s_activeCamera = camera; }

   private:
    glm::vec3 m_lookTarget = vec3(0, 0, -1);

    float m_fov = 60.0f;

    float m_zNear = 0.1f;
    float m_zFar = 2000.0f;

    float m_moveSpeed = 5;
    float m_moveSpeedMultiplier = 4;

    inline static std::optional<std::reference_wrapper<Camera>> s_activeCamera = std::nullopt;
};

}  // namespace components
}  // namespace beet
