
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

    log::debug("Vendor graphic card: {}", glGetString(GL_VENDOR));
    log::debug("Renderer: {}", glGetString(GL_RENDERER));
    log::debug("Version GL: {}", glGetString(GL_VERSION));
    log::debug("Version GLSL: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLint nExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);

    // for (int i = 0; i < nExtensions; i++) {
    //     log::debug("[{}] : {}", i, glGetStringi(GL_EXTENSIONS, i));
    // }
}

void Renderer::on_awake() {
    const auto size = m_engine.get_window_module().lock()->get_window_size();
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
    {
        m_depthProgram.load_shader("depth", "depth.vert", "depth.frag");
        m_depthModelUniform = glGetUniformLocation(m_depthProgram.get_program(), "model");
    }
    {
        m_pickingProgram.load_shader("picking", "picking.vert", "picking.frag");
        m_pickingModelUniform = glGetUniformLocation(m_pickingProgram.get_program(), "model");
        m_pickingenttHandleUniform = glGetUniformLocation(m_pickingProgram.get_program(), "enttHandle");

        GLuint uboMatrixIndex = glGetUniformBlockIndex(m_pickingProgram.get_program(), "Matrices");
        glUniformBlockBinding(m_pickingProgram.get_program(), uboMatrixIndex, 0);
    }
    {
        m_plane = std::make_shared<components::InstanceMesh>("plane");
        m_plane->on_awake();
    }
    {
        m_compositeProgram.set_asset_name("oit composite");
        m_compositeProgram.load_shader("oit", "oit_composite.vert", "oit_composite.frag");
    }

    glCreateBuffers(1, &fragmentstartindexinitializerbuffer);
    glCreateBuffers(1, &fragmentbuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, fragmentbuffer);

    glCreateBuffers(1, &fragmentindexbuffer);
    glNamedBufferData(fragmentindexbuffer, sizeof(uint), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, fragmentindexbuffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &fragmentstartindextexture);
    glTextureStorage2D(fragmentstartindextexture, 1, GL_R32UI, size.x, size.y);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // bind to image unit 6
    //    glBindImageTexture(6, fragmentstartindextexture, 0, false, 0, GL_READ_WRITE, GL_R32UI);
    resize_all_framebuffers(size);
}

void Renderer::on_update(double deltaTime) {
    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->clear_all_framebuffers();

    m_timePassed += (float)deltaTime;

    if (glm::isnan((float)m_engine.get_window_module().lock()->get_window_aspect_ratio())) {
        return;
    }

    update_universal_buffer_data();
    depth_pass();
    picking_pass();
    shadow_pass();
    opaque_pass();
    transparent_pass();
    post_process_pass();
    gui_pass();
    back_buffer_pass();
}

void Renderer::update_universal_buffer_data() {
    using namespace components;

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

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
}

void Renderer::depth_pass() {
    using namespace components;
    auto fbm = m_engine.get_framebuffer_module().lock();

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    auto entities = registry.view<InstanceMesh, Transform>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        InstanceMesh& instanceMesh = go.get_component<InstanceMesh>();
        Transform& transform = go.get_component<Transform>();

        fbm->bind_framebuffer(FrameBufferType::Depth);
        glUseProgram(m_depthProgram.get_program());

        glUniformMatrix4fv(m_depthModelUniform, 1, GL_FALSE, value_ptr(transform.get_model_matrix()));

        instanceMesh.draw();

        glUseProgram(0);
        fbm->unbind_framebuffer();
    }
}

void Renderer::picking_pass() {
    using namespace components;
    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->bind_framebuffer(FrameBufferType::ObjectPicking);
    // TODO cannot have color as 0,0,0,0 as that is a valid entt handle
    glClearColor(1.0f, 1.0, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);

    glUseProgram(m_pickingProgram.get_program());
    auto entities = registry.view<InstanceMesh, Transform>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        InstanceMesh& instanceMesh = go.get_component<InstanceMesh>();
        Transform& transform = go.get_component<Transform>();

        glUniformMatrix4fv(m_pickingModelUniform, 1, GL_FALSE, value_ptr(transform.get_model_matrix()));
        auto handle = (uint32_t)go.get_handle();
        glUniform1i(m_pickingenttHandleUniform, handle);
        instanceMesh.draw();
    }
    glUseProgram(0);
    fbm->unbind_framebuffer();

    glClearColor(m_clearCol.x, m_clearCol.y, m_clearCol.z, m_clearCol.w);
}

void Renderer::opaque_pass() {
    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->bind_framebuffer(FrameBufferType::Opaque);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    using namespace components;

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto skyboxes = registry.view<SkyBox>();
    for (auto& e : skyboxes) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        SkyBox& skybox = go.get_component<SkyBox>();

        glDepthFunc(GL_EQUAL);
        glCullFace(GL_BACK);

        skybox.set_uniforms();
        skybox.draw();
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glCullFace(GL_NONE);
    glDisable(GL_CULL_FACE);

    auto entities = registry.view<Transform, InstanceMesh, Material>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        InstanceMesh& mesh = go.get_component<InstanceMesh>();
        Material& material = go.get_component<Material>();

        if (material.get_is_opaque() == false) {
            continue;
        }

        glm::mat4 model = transform.get_model_matrix();

        material.set_uniforms(model);
        mesh.draw();
    }
    fbm->unbind_framebuffer();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Renderer::transparent_pass() {
    const auto size = m_engine.get_window_module().lock()->get_window_size();

    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->bind_framebuffer(FrameBufferType::Transparency);

    using namespace components;

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glBindImageTexture(6, fragmentstartindextexture, 0, false, 0, GL_READ_WRITE, GL_R32UI);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, fragmentstartindexinitializerbuffer);
    glTextureSubImage2D(fragmentstartindextexture, 0, 0, 0, size.x, size.y, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

    // initialize fragment index buffer
    uint atomic_fragment_index = 0;
    glNamedBufferSubData(fragmentindexbuffer, 0, sizeof(uint), &atomic_fragment_index);

    auto entities = registry.view<Transform, InstanceMesh, Material>();
    for (auto& e : entities) {
        glClear(GL_DEPTH_BUFFER_BIT);
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        InstanceMesh& mesh = go.get_component<InstanceMesh>();
        Material& material = go.get_component<Material>();
        if (material.get_is_opaque() == true) {
            continue;
        }

        glm::mat4 model = transform.get_model_matrix();

        material.set_uniforms(model);

        mesh.draw();
        glUseProgram(0);
    }
    fbm->unbind_framebuffer();

    // SORT / COMPOSE

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fbm->bind_framebuffer(FrameBufferType::Opaque);
    glUseProgram(m_compositeProgram.get_program());
    m_plane->draw();
    fbm->unbind_framebuffer();
    glUseProgram(0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glDisable(GL_BLEND);
    fbm->unbind_framebuffer();
}

void Renderer::post_process_pass() {}

void Renderer::back_buffer_pass() {
    using namespace components;
    auto fbm = m_engine.get_framebuffer_module().lock();
    fbm->bind_framebuffer(FrameBufferType::Back);

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    auto entities = registry.view<PostProcessing>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        PostProcessing& postProcessing = go.get_component<PostProcessing>();

        fbm->bind_framebuffer(FrameBufferType::Back);

        auto tex = fbm->get_color_attachment(FrameBufferType::Opaque);

        postProcessing.set_target_texture(tex);
        postProcessing.apply_post_processing();
        postProcessing.draw();
    }
}
struct Fragment {
    uint NextIndex;
    uint Color;
    float Depth;
    float _pad;
};
void Renderer::resize_all_framebuffers(const vec2i& size) {
    glViewport(0, 0, size.x, size.y);

    glDeleteTextures(1, &fragmentstartindextexture);

    glCreateTextures(GL_TEXTURE_2D, 1, &fragmentstartindextexture);

    glTextureStorage2D(fragmentstartindextexture, 1, GL_R32UI, size.x, size.y);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(fragmentstartindextexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // bind to image unit 1

    glBindImageTexture(1, fragmentstartindextexture, 0, false, 0, GL_READ_WRITE, GL_R32UI);
    // resize fragment start index initializer buffer
    std::vector<uint> fragmentstartindexinitializer(size.x * size.y, 0xFFFFFFFF);

    glNamedBufferData(fragmentstartindexinitializerbuffer, fragmentstartindexinitializer.size() * sizeof(uint),
                      fragmentstartindexinitializer.data(), GL_DYNAMIC_COPY);
    // resize fragmentbuffer too
    glNamedBufferData(fragmentbuffer, size.x * size.y * 30 * sizeof(Fragment), nullptr, GL_DYNAMIC_DRAW);
    //    glNamedBufferData(fragmentbuffer, size.x * size.y * 10 * sizeof(uvec4), nullptr, GL_DYNAMIC_DRAW);

    m_engine.get_framebuffer_module().lock()->resize_all_framebuffers(size);
}

void Renderer::shadow_pass() {}
void Renderer::gui_pass() {}

void Renderer::on_late_update() {}

void Renderer::on_destroy() {
    log::debug("Renderer destroyed");
}

Renderer::~Renderer() {}

}  // namespace beet
