
#include <beet/assert.h>
#include <beet/components.h>
#include <beet/engine.h>
#include <beet/renderer.h>
#include <beet/scene.h>
#include <beet/types.h>
#include <glad/glad.h>

namespace beet {

Renderer::Renderer(Engine& engine) : m_engine(engine) {
    auto loaded = gladLoadGL();
    BEET_ASSERT_MESSAGE(loaded, "Failed to initialize GLAD");
    log::debug("GLAD initialized");
}

void Renderer::on_awake() {
    auto width = m_engine.get_window_module().lock()->get_window_width();
    auto height = m_engine.get_window_module().lock()->get_window_height();
    recreate_framebuffer(width, height);

    glClearColor(m_clearCol.x, m_clearCol.y, m_clearCol.z, m_clearCol.w);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_bufferData.init();
}

Renderer::~Renderer() {}

void Renderer::on_update(double deltaTime) {
    clear_framebuffer(0);
    m_timePassed += (float)deltaTime;

    if (glm::isnan((float)m_engine.get_window_module().lock()->get_window_aspect_ratio())) {
        return;
    }

    depth_pass(0);
    shadow_pass(0);
    color_pass(0);
    transparent_pass(0);
    post_process_pass(0);
    gui_pass(0);
}

void Renderer::shadow_pass(uint16_t id) {}
void Renderer::depth_pass(uint16_t id) {}
void Renderer::color_pass(uint16_t id) {
    using namespace components;

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    //=CAMERA===========================
    auto cameras = registry.view<Transform, Camera, Name>();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    for (auto& cam : cameras) {
        auto goOpt = scene.get_game_object_from_handle(cam);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        Camera& camera = go.get_component<Camera>();
        Name& name = go.get_component<Name>();

        const glm::vec3 pos = transform.get_position();
        const glm::vec3 lookTarget = camera.get_look_target();
        const glm::vec3 up = transform.up();

        const float fovY = camera.get_fov();
        const float zNear = camera.get_z_near();
        const float zFar = camera.get_z_far();

        const float aspectRatio = (float)m_engine.get_window_module().lock()->get_window_aspect_ratio();

        view = glm::lookAt(pos, lookTarget, up);
        proj = glm::perspective(fovY, aspectRatio, zNear, zFar);

        m_bufferData.update_view_projection_data(view, proj, pos);
    }

    auto lights = registry.view<Transform, PointLight>();
    std::vector<PackedPointLightData> m_lightData;
    for (auto& e : lights) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }
        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        PointLight& pointLight = go.get_component<PointLight>();

        PackedPointLightData data{};
        data.pointPosition_pointRange = vec4(transform.get_position(), pointLight.get_range());
        data.pointColor_pointIntensity = vec4(pointLight.get_color(), pointLight.get_intensity());
        m_lightData.emplace_back(data);
    }

    //    for (int i = 0; i < 300; ++i) {
    //        m_lightData.emplace_back(PackedPointLightData{vec4(1), vec4(1)});
    //    }

    m_bufferData.update_point_light_data(m_lightData);

    auto entities = registry.view<Transform, InstanceMesh, Name, Material>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        InstanceMesh& mesh = go.get_component<InstanceMesh>();
        Material& material = go.get_component<Material>();
        Name& name = go.get_component<Name>();

        glm::mat4 model = transform.get_model_matrix();
        material.set_uniforms(model, view, proj);

        //=RENDER=======================
        glCullFace(GL_BACK);
        mesh.draw();
    }
}
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

void Renderer::on_late_update() {}

void Renderer::on_destroy() {
    log::debug("Renderer destroyed");
}

}  // namespace beet
