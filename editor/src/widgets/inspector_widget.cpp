#include "inspector_widget.h"
#include <beet/scene.h>
#include <imgui.h>

namespace beet {

InspectorWidget::InspectorWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

void InspectorWidget::on_widget_render() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(m_name.c_str());

    render_inspector();

    ImGui::End();
}

void InspectorWidget::render_inspector() {
    using namespace components;
    auto selected = m_editorWidgets.get_selected_entity();

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }

    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto goOpt = scene.get_game_object_from_id(selected);
    if (!goOpt) {
        return;
    }

    GameObject go = goOpt.value();

    m_handle = std::to_string(static_cast<size_t>(go.get_handle()));
    m_uuid = uuids::to_string(go.get_id());

    //=Render Inspector===========

    auto name = registry.try_get<components::Name>(go.get_handle());
    if (name) {
        render_name_component(*name);
    }

    ImGui::Separator();

    auto transform = registry.try_get<components::Transform>(go.get_handle());
    if (transform) {
        render_transform_component(*transform);
    }

    ImGui::Separator();
}

void InspectorWidget::render_name_component(components::Name& name) {
    char entityName[64]{};
    sprintf_s(entityName, "%s", name.name.c_str());

    ImGui::InputText("##INSPECTOR_NAME", entityName, IM_ARRAYSIZE(entityName));
    name.name = entityName;
}
void InspectorWidget::render_transform_component(components::Transform& transform) {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        vec3 position = transform.get_position();
        vec3 rotation = transform.get_rotation_euler();
        vec3 scale = transform.get_scale();

        ImGui::Text("Transform");

        ImGui::Text("Position : ");
        ImGui::SameLine();
        ImGui::DragFloat3("##P", &position.x, -0.1f, 0.1f);

        ImGui::Text("Rotation : ");
        ImGui::SameLine();
        ImGui::DragFloat3("##R", &rotation.x, -1.0f, 1.0f);

        ImGui::Text("Scale :    ");
        ImGui::SameLine();
        ImGui::DragFloat3("##S", &scale.x, -0.1f, 0.1f);

        transform.set_position(position);
        transform.set_rotation_euler(rotation);
        transform.set_scale(scale);
    }
    ImGui::Separator();
}

InspectorWidget::~InspectorWidget() {
    log::debug("InspectorWidget destroyed");
}

}  // namespace beet
