#include <beet/components.h>
#include <beet/editor_camera.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/scene.h>
#include <fstream>
#include <string_view>

namespace beet {
EditorCameraController::EditorCameraController(Engine& engine) : m_engine(engine) {
    m_inputManager = m_engine.get_window_module().lock()->get_input_manager();
}

void EditorCameraController::on_awake() {
    m_engine.get_window_module().lock()->set_cursor_hide(m_lockState);

    using namespace components;
    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }

    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();
    auto cameras = registry.view<Transform, Camera, Name>();

    for (auto& cam : cameras) {
        auto goOpt = scene.get_game_object_from_handle(cam);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();

        auto rotation = transform.get_rotation_euler();

        m_pitch = rotation.x;
        m_yaw = rotation.y;
        m_roll = rotation.z;
    }
}
void EditorCameraController::on_update(double deltaTime) {
    using namespace components;

    camera_key_input();

    bool skipCameraBehaviour = false;

    vec2d currentMousePos = m_inputManager->get_absolute_position();
    if (glm::isinf(currentMousePos.x) || glm::isinf(currentMousePos.y) || !m_lockState) {
        skipCameraBehaviour = true;
    }

    m_mouseDelta = (currentMousePos - m_lastMousePosition);

    auto cameraOpt = Camera::get_active_camera();
    if (!cameraOpt) {
        skipCameraBehaviour = true;
    }

    auto& editorCamera = cameraOpt.value().get();

    auto goOpt = GameObject::get_game_object_from_component(editorCamera);
    if (!goOpt) {
        skipCameraBehaviour = true;
    }
    if (!skipCameraBehaviour) {
        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        Name& name = go.get_component<Name>();

        auto rotation = transform.get_rotation_euler();

        m_pitch = rotation.x;
        m_yaw = rotation.y;
        m_roll = rotation.z;

        //= CAMERA ROTATION
        m_roll = 0.0f;
        m_pitch -= ((float)m_mouseDelta.y * (float)m_mouseSensitivity.y) * (float)deltaTime;
        m_yaw -= ((float)m_mouseDelta.x * (float)m_mouseSensitivity.x) * (float)deltaTime;

        m_pitch = clamp(m_pitch, radians(-80.f), radians(80.f));
        transform.set_rotation_euler(vec3(m_pitch, m_yaw, m_roll));

        //= CALCULATE FORWARD VECTOR == // TODO STORE / CALC THESE IN TRANSFORM
        glm::vec3 look;
        look.x = cosf(m_pitch) * sinf(m_yaw);
        look.y = sinf(m_pitch);
        look.z = cosf(m_pitch) * cosf(m_yaw);

        m_forward = normalize(look);

        //= CALCULATE LOCAL RIGHT AND UP == // TODO STORE / CALC THESE IN TRANSFORM
        m_right = normalize(cross(m_forward, vec3(0, 1, 0)));
        m_up = normalize(cross(m_right, m_forward));

        //= CAMERA MOVEMENT
        vec3 position = transform.get_position();
        vec3 nextPosition = position + (m_keyboardDirection * m_movementMultiplier * m_moveSpeed * (float)deltaTime);
        transform.set_position(nextPosition);

        //= SET LOOK TARGET POSITION USING FORWARD VECTOR
        vec3 targetPos = transform.get_position() + m_forward;
        editorCamera.set_look_target(targetPos);
    }

    m_lastMousePosition = currentMousePos;
}

void EditorCameraController::camera_key_input() {
    //=Camera state=== //TODO when holding left click on the active window
    if (m_inputManager->key_pressed(KeyCode::E)) {
        if (m_ePressed) {
            m_lockState = !m_lockState;
            m_engine.get_window_module().lock()->set_cursor_hide(m_lockState);
        }
        m_ePressed = false;
    } else {
        m_ePressed = true;
    }

    //= XYZ editor movement input

    vec3 movementDirectionXYZ = vec3(0);

    if (m_inputManager->key_pressed(KeyCode::W)) {
        movementDirectionXYZ += m_forward;
    }
    if (m_inputManager->key_pressed(KeyCode::S)) {
        movementDirectionXYZ += -m_forward;
    }
    if (m_inputManager->key_pressed(KeyCode::A)) {
        movementDirectionXYZ += -m_right;
    }
    if (m_inputManager->key_pressed(KeyCode::D)) {
        movementDirectionXYZ += m_right;
    }
    if (m_inputManager->key_pressed(KeyCode::R)) {
        movementDirectionXYZ += WORLD_UP;
    }
    if (m_inputManager->key_pressed(KeyCode::F)) {
        movementDirectionXYZ += -WORLD_UP;
    }

    vec3 speedTarget;
    if (m_inputManager->key_pressed(KeyCode::LeftShift)) {
        speedTarget = m_maxMovementMultiplier;
    } else {
        speedTarget = m_minMovementMultiplier;
    }
    m_movementMultiplier = lerp(m_movementMultiplier, speedTarget, .15f);
    m_keyboardDirection = movementDirectionXYZ;
}

void EditorCameraController::on_late_update() {}
void EditorCameraController::on_destroy() {}

}  // namespace beet
