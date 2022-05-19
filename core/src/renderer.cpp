
#include <beet/assert.h>
#include <beet/components.h>
#include <beet/engine.h>
#include <beet/framebuffer_manager.h>
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
    auto size = m_engine.get_window_module().lock()->get_window_size();
    resize_all_framebuffers(size);

    glClearColor(m_clearCol.x, m_clearCol.y, m_clearCol.z, m_clearCol.w);

    glEnable(GL_MULTISAMPLE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    glClearStencil(0);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_universalBufferData.init();
}

Renderer::~Renderer() {}

void Renderer::on_update(double deltaTime) {
    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->clear_all_framebuffers();

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
    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->bind_framebuffer(FrameBufferType::Color);
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
        m_universalBufferData.update_view_projection_data(view, proj, pos);
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
    // TODO consider passing in as 2 arrays/vectors as size and intensity is unlikely to be updated as often as position
    m_universalBufferData.update_point_light_data(m_lightData);

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
        material.set_uniforms(model);

        glCullFace(GL_BACK);
        mesh.draw();
    }
    fbm->unbind_framebuffer();
    //    m_tempFramebufferColor.unbind();
}
void Renderer::gui_pass(uint16_t id) {}
void Renderer::transparent_pass(uint16_t id) {}
void Renderer::post_process_pass(uint16_t id) {
    using namespace components;
    auto fbm = m_engine.get_framebuffer_module().lock();

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto entities = registry.view<InstanceMesh, PostProcessing>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        InstanceMesh& mesh = go.get_component<InstanceMesh>();
        PostProcessing& postProcessing = go.get_component<PostProcessing>();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        auto tex = fbm->get_color_attachment(FrameBufferType::Color);
        postProcessing.set_target_texture(tex);
        postProcessing.apply_post_processing();

        mesh.draw();
        glEnable(GL_DEPTH_TEST);
    }
}

void Renderer::resize_all_framebuffers(const vec2i& size) {
    glViewport(0, 0, size.x, size.y);
    m_engine.get_framebuffer_module().lock()->resize_all_framebuffers(size);
}

void Renderer::on_late_update() {}

void Renderer::on_destroy() {
    log::debug("Renderer destroyed");
}

}  // namespace beet
