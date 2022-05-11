#include <beet/transform.h>

namespace beet {
namespace components {

Transform::Transform(const vec3& position, const vec3& scale, const vec3& rotation)
    : m_position(position), m_scale(scale), m_rotation(rotation) {}

quat Transform::get_rotation() const {
    return quat(degrees(m_rotation));
}

vec3 Transform::get_rotation_euler() const {
    return degrees(m_rotation);
}

void Transform::set_rotation(const quat& rotation) {
    m_rotation = radians(eulerAngles(rotation));
}

void Transform::set_rotation_euler(const vec3& euler) {
    m_rotation = radians(euler);
}

vec3 Transform::get_position() const {
    return m_position;
}

void Transform::set_position(const vec3& position) {
    m_position = position;
}

vec3 Transform::get_scale() const {
    return m_scale;
}

void Transform::set_scale(const vec3& scale) {
    m_scale = scale;
}

mat4 Transform::get_model_matrix() {
    mat4 modelMatrix{1.0f};
    modelMatrix = translate(mat4(1.0f), m_position) * toMat4(quat(m_rotation)) * scale(mat4(1.0f), m_scale);
    return modelMatrix;
}

vec3 Transform::forward() const {
    constexpr vec3 forward = vec3(0, 0, 1);
    return quat(m_rotation) * forward;
}

vec3 Transform::up() const {
    constexpr vec3 up = vec3(0, 1, 0);
    return quat(m_rotation) * up;
}

vec3 Transform::right() const {
    constexpr vec3 right = vec3(1, 0, 0);
    return quat(m_rotation) * right;
}

}  // namespace components
}  // namespace beet
