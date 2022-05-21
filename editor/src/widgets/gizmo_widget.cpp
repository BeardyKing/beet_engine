#include "gizmo_widget.h"

namespace beet {
GizmoWidget::GizmoWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

GizmoWidget::~GizmoWidget() {}

void GizmoWidget::on_widget_render() {
    if (glm::isnan((float)m_editorWidgets.get_engine().get_window_module().lock()->get_window_aspect_ratio())) {
        return;
    }

    auto fbm = m_editorWidgets.get_engine().get_framebuffer_module().lock();
    auto fbo = fbm->get_framebuffer(FrameBufferType::Color);

    if (!fbo.get_framebuffer()) {
        return;
    }

    ImGuizmo::BeginFrame();
    ImGui::Begin(m_name.c_str(), &m_isActive, ImGuiWindowFlags_MenuBar);

    render_editor_scene(fbo);

    ImGui::End();
}

void GizmoWidget::render_editor_scene(Framebuffer& fbo) {
    auto colorTexture = fbo.get_color_texture();

    auto rawSize = fbo.get_size();
    float aspectX = (float)rawSize.y / (float)rawSize.x;
    float width = ImGui::GetWindowContentRegionWidth();
    float height = width * aspectX;

    ImVec2 imageSize(width, height);

    auto imagePos = ImGui::GetCursorScreenPos();

    if (colorTexture) {
        ImVec2 uv_min = ImVec2(0.0f, 0.0f);
        ImVec2 uv_max = ImVec2(1.0f, -1.0f);
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

        ImGui::Image((GLuint*)colorTexture, imageSize, uv_min, uv_max, tint_col, border_col);
    }

    render_gizmo(imagePos, imageSize);
}

void GizmoWidget::render_gizmo(const ImVec2& windowPos, const ImVec2& imageSize) {
    using namespace components;

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }
    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto cameras = registry.view<Transform, Camera>();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    vec2i windowSize{0};
    for (auto& cam : cameras) {
        auto goOpt = scene.get_game_object_from_handle(cam);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Transform& transform = go.get_component<Transform>();
        Camera& camera = go.get_component<Camera>();

        const glm::vec3 pos = transform.get_position();
        const glm::vec3 lookTarget = camera.get_look_target();
        const glm::vec3 up = transform.up();

        const float fovY = camera.get_fov();
        const float zNear = camera.get_z_near();
        const float zFar = camera.get_z_far();

        windowSize = m_editorWidgets.get_engine().get_window_module().lock()->get_window_size();
        const float aspectRatio =
            (float)m_editorWidgets.get_engine().get_window_module().lock()->get_window_aspect_ratio();

        view = glm::lookAt(pos, lookTarget, up);
        proj = glm::perspective(fovY, aspectRatio, zNear, zFar);
    }
    // TODO consider storing this data locally in uniform buffer data class located in renderer
    // instead of only storing view / proj on the gpu

    auto selected = m_editorWidgets.get_selected_entity();
    auto goOpt = scene.get_game_object_from_id(selected);
    if (!goOpt) {
        return;
    }

    GameObject go = goOpt.value();

    if (!go.has_component<components::Transform>()) {
        return;
    }
    auto& transform = go.get_component<components::Transform>();
    auto model = transform.get_model_matrix();

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(windowPos.x, windowPos.y, imageSize.x, imageSize.y);

    // TODO setup buttons
    // TODO set imgui window to size of active render window / editor window

    if (ImGui::IsKeyPressed(49))
        m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;  // key "1"
    if (ImGui::IsKeyPressed(50))
        m_currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;  // key "2"
    if (ImGui::IsKeyPressed(51))
        m_currentGizmoOperation = ImGuizmo::OPERATION::SCALE;  // key "3"
    if (ImGui::IsKeyPressed(52))
        m_currentGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;  // key "4"

    vec3 position{0};
    vec3 rotation{0};
    vec3 scale{0};
    mat4 mtx = transform.get_model_matrix();

    // clang-format off

    ImGuizmo::Manipulate(
        value_ptr(view),
        value_ptr(proj),
        m_currentGizmoOperation,
        m_currentGizmoMode,
        value_ptr(mtx),
        0,
        NULL
    );

    ImGuizmo::DecomposeMatrixToComponents(
        value_ptr(mtx),
        value_ptr(position),
        value_ptr(rotation),
        value_ptr(scale)
    );

    // clang-format on

    if (ImGuizmo::IsUsing()) {
        transform.set_position(position);
        transform.set_rotation_euler(rotation);
        transform.set_scale(scale);
    }
}

}  // namespace beet