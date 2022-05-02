
#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/types.h>
#include <glad/glad.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    auto loaded = gladLoadGL();
    BEET_ASSERT_MESSAGE(loaded, "Failed to initialize GLAD");
    log::debug("GLAD initialized");
}

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
unsigned int VBO;
unsigned int VAO;

void Renderer::on_awake() {
    auto width = m_engine.get_window_module().lock()->get_window_width();
    auto height = m_engine.get_window_module().lock()->get_window_height();
    recreate_framebuffer(width, height);

    glClearColor(m_clearCol.x, m_clearCol.y, m_clearCol.z, m_clearCol.w);

    m_testMesh = std::make_shared<components::Mesh>("default_cube.obj");
    m_testMesh->on_awake();

    m_testShader = std::make_shared<components::ShaderProgram>();
    m_testShader->set_asset_name("fallback");
    m_testShader->load_shader("fallback", "fallback.vert", "fallback.frag");

    m_testTexture = std::make_shared<components::Texture>("UV_Grid_test.png");
    m_testTexture->on_awake();

    //=Camera=========
    m_testCameraTransform = std::make_shared<components::Transform>();
    m_testCameraTransform->set_position(vec3(0, 0, -5));
    m_testCamera = std::make_shared<components::Camera>();
    m_testCamera->on_awake();
    //================

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //=setup uniforms

    m_modelLoc = glGetUniformLocation(m_testShader->get_program(), "model");
    m_viewLoc = glGetUniformLocation(m_testShader->get_program(), "view");
    m_projLoc = glGetUniformLocation(m_testShader->get_program(), "projection");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

Renderer::~Renderer() {}

void Renderer::on_update(double deltaTime) {
    clear_framebuffer(0);
    const float aspectRatio = (float)m_engine.get_window_module().lock()->get_window_aspect_ratio();
    if (glm::isnan(aspectRatio)) {
        return;
    }
    m_timePassed += (float)deltaTime;

    auto input = m_engine.get_window_module().lock()->get_input_manager().get();

    //=Camera state===
    if (input->key_pressed(KeyCode::E)) {
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

    if (input->key_pressed(KeyCode::W)) {
        movementDirectionXYZ += m_forward;
    }
    if (input->key_pressed(KeyCode::S)) {
        movementDirectionXYZ += -m_forward;
    }
    if (input->key_pressed(KeyCode::A)) {
        movementDirectionXYZ += -m_right;
    }
    if (input->key_pressed(KeyCode::D)) {
        movementDirectionXYZ += m_right;
    }
    if (input->key_pressed(KeyCode::R)) {
        movementDirectionXYZ += WORLD_UP;
    }
    if (input->key_pressed(KeyCode::F)) {
        movementDirectionXYZ += -WORLD_UP;
    }

    vec3 speedTarget;
    if (input->key_pressed(KeyCode::LeftShift)) {
        speedTarget = m_maxMovementMultiplier;
    } else {
        speedTarget = m_minMovementMultiplier;
    }
    m_movementMultiplier = lerp(m_movementMultiplier, speedTarget, .15f);
    m_keyboardDirection = movementDirectionXYZ;

    bool skipCameraBehaviour = false;

    vec2d currentMousePos = input->get_absolute_position();
    if (glm::isinf(currentMousePos.x) || glm::isinf(currentMousePos.y)) {
        skipCameraBehaviour = true;
    }
    m_mouseDelta = (currentMousePos - m_lastMousePosition);
    if (m_lockState == false) {
        skipCameraBehaviour = true;
    }

    auto rotation = m_testCameraTransform->get_rotation_euler();

    m_pitch = rotation.x;
    m_yaw = rotation.y;
    m_roll = rotation.z;

    if (skipCameraBehaviour == false) {
        //= CAMERA ROTATION
        m_roll = 0.0f;
        m_pitch -= ((float)m_mouseDelta.y * (float)m_mouseSensitivity.y) * (float)deltaTime;
        m_yaw -= ((float)m_mouseDelta.x * (float)m_mouseSensitivity.x) * (float)deltaTime;

        if (deltaTime > 0.009f) {
            log::info("DT: {}", deltaTime);
        }
        m_pitch = clamp(m_pitch, radians(-80.f), radians(80.f));
        m_testCameraTransform->set_rotation_euler(vec3(m_pitch, m_yaw, m_roll));

        //= CALCULATE FORWARD VECTOR == // TODO STORE / CALC THESE IN TRANSFORM
        glm::vec3 look;
        look.x = cosf(m_pitch) * sinf(m_yaw);
        look.y = sinf(m_pitch);
        look.z = cosf(m_pitch) * cosf(m_yaw);

        m_forward = glm::normalize(look);
        //        vec3 look;
        //        look.x = cosf(radians(m_yaw)) * cosf(radians(m_pitch));
        //        look.y = sinf(radians(m_pitch));
        //        look.z = sinf(radians(m_yaw)) * cosf(radians(m_pitch));

        m_forward = normalize(look);

        //= CALCULATE LOCAL RIGHT AND UP == // TODO STORE / CALC THESE IN TRANSFORM
        m_right = normalize(cross(m_forward, vec3(0, 1, 0)));
        m_up = normalize(cross(m_right, m_forward));

        //= CAMERA MOVEMENT
        vec3 position = m_testCameraTransform->get_position();
        vec3 nextPosition = position + (m_keyboardDirection * m_movementMultiplier * m_moveSpeed * (float)deltaTime);
        m_testCameraTransform->set_position(nextPosition);

        //= SET LOOK TARGET POSITION USING FORWARD VECTOR
        vec3 targetPos = m_testCameraTransform->get_position() + m_forward;
        m_testCamera->set_look_target(targetPos);
    }

    m_lastMousePosition = currentMousePos;

    //=Camera=========

    const glm::vec3 pos = m_testCameraTransform->get_position();
    const glm::vec3 lookTarget = m_testCamera->get_look_target();
    const glm::vec3 up = m_testCameraTransform->up();

    const float fovY = m_testCamera->get_fov();
    const float zNear = m_testCamera->get_z_near();
    const float zFar = m_testCamera->get_z_far();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    // Set view and projection matrix for view 0.
    {
        view = glm::lookAt(pos, lookTarget, up);
        proj = glm::perspective(fovY, aspectRatio, zNear, zFar);
    }

    //=Test cube======

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f));

    glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    //=RENDER=======================

    glCullFace(GL_BACK);
    m_testTexture->bind();
    glUseProgram(m_testShader->get_program());
    m_testMesh->draw();

    depth_pass(0);
    shadow_pass(0);
    color_pass(0);
    transparent_pass(0);
    post_process_pass(0);
    gui_pass(0);
}
void Renderer::on_late_update() {}
void Renderer::on_destroy() {}

void Renderer::shadow_pass(uint16_t id) {}
void Renderer::depth_pass(uint16_t id) {}
void Renderer::color_pass(uint16_t id) {}
void Renderer::gui_pass(uint16_t id) {}
void Renderer::transparent_pass(uint16_t id) {}
void Renderer::post_process_pass(uint16_t id) {}

void Renderer::recreate_framebuffer(uint16_t width, uint16_t height, uint16_t id) {
    glViewport(0, 0, width, height);
}

void Renderer::clear_framebuffer(uint16_t id) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Renderer::clear_all_framebuffer_objects() {
    // TODO when framebuffer manager in impl
    clear_framebuffer(0);
}

}  // namespace beet
