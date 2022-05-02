#include <beet/camera.h>

namespace beet {
namespace components {

Camera::Camera() {}
Camera::~Camera() {
    on_destroy();  // fallback
}

void Camera::on_awake() {
    s_activeCamera = *this;
}

void Camera::on_destroy() {
    if (!s_activeCamera)
        return;
    if (&s_activeCamera.value().get() != this)
        return;

    s_activeCamera = std::nullopt;
}

vec3 Camera::world_up() {
    constexpr vec3 WORLD_UP = vec3(0, 1, 0);
    return WORLD_UP;
}

}  // namespace components
}  // namespace beet
