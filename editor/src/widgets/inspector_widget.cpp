#include "inspector_widget.h"
#include <beet/scene.h>

namespace beet {

InspectorWidget::InspectorWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

void InspectorWidget::on_widget_render() {
    using namespace components;
    auto selected = m_editorWidgets.get_selected_entity();

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(m_name.c_str());

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        ImGui::End();
        return;
    }

    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto goOpt = scene.get_game_object_from_id(selected);
    if (!goOpt) {
        ImGui::End();
        return;
    }

    GameObject go = goOpt.value();

    m_handle = std::to_string(static_cast<size_t>(go.get_handle()));
    m_uuid = uuids::to_string(go.get_id());

    auto name = registry.try_get<components::Name>(go.get_handle());
    if (name) {
        render_name_component(*name);
    }
    ImGui::Separator();

    ImGui::End();
}
void InspectorWidget::render_name_component(components::Name& name) {
    char entityName[64]{};
    sprintf_s(entityName, "%s", name.name.c_str());

    ImGui::InputText("##INSPECTOR_NAME", entityName, IM_ARRAYSIZE(entityName));
    name.name = entityName;
}

InspectorWidget::~InspectorWidget() {
    log::debug("InspectorWidget destroyed");
}

}  // namespace beet
