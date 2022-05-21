#include "gizmo_widget.h"

namespace beet {
GizmoWidget::GizmoWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

GizmoWidget::~GizmoWidget() {}

void GizmoWidget::on_widget_render() {
    if (glm::isnan((float)m_editorWidgets.get_engine().get_window_module().lock()->get_window_aspect_ratio())) {
        return;
    }

    ImGuizmo::BeginFrame();
    auto windowSize = m_editorWidgets.get_engine().get_window_module().lock()->get_window_size();

    ImGui::Begin(m_name.c_str(), NULL,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);

    render_gizmo();

    ImGui::End();
}
void GizmoWidget::render_gizmo() {
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
    ImGuizmo::SetRect(0, 0, windowSize.x, windowSize.y);

    // TODO setup buttons
    // TODO set imgui window to size of active render window / editor window

    if (ImGui::IsKeyPressed(49))
        m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;  // key "1"
    if (ImGui::IsKeyPressed(50))
        m_currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;  // key "2"
    if (ImGui::IsKeyPressed(51))
        m_currentGizmoOperation = ImGuizmo::OPERATION::SCALE;  // key "3"
    if (ImGui::IsKeyPressed(52))
        m_currentGizmoOperation = ImGuizmo::OPERATION::BOUNDS;  // key "4"

    vec3 position = transform.get_position();
    vec3 rotation = transform.get_rotation_euler();
    vec3 scale = transform.get_scale();
    mat4 mtx{};

    // clang-format off

    ImGuizmo::RecomposeMatrixFromComponents(
        value_ptr(position),
        value_ptr(rotation),
        value_ptr(scale),
        value_ptr(mtx)
    );

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