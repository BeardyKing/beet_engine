#include "hierarchy_widget.h"
#include <beet/scene.h>

namespace beet {

HierarchyWidget::HierarchyWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

void HierarchyWidget::on_widget_render() {
    using namespace components;
    auto selected = m_editorWidgets.get_selected_entity();

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    ImGui::Begin(m_name.c_str());
    
    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }

    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto entities = registry.view<Name>();
    for (auto& e : entities) {
        auto goOpt = scene.get_game_object_from_handle(e);
        if (!goOpt) {
            continue;
        }

        GameObject go = goOpt.value();
        Name& name = go.get_component<Name>();

        auto label = name.name;
        if (ImGui::Selectable(label.c_str(), selected == go.get_id())) {
            selected = go.get_id();
            m_editorWidgets.set_selected_entity(selected);
        }
    }

    ImGui::End();
}

HierarchyWidget::~HierarchyWidget() {
    log::debug("HierarchyWidget destroyed");
}

}  // namespace beet
