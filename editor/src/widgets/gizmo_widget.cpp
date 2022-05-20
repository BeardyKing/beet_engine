#include "gizmo_widget.h"
#include <ImGuizmo.h>
namespace beet {
GizmoWidget::GizmoWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

GizmoWidget::~GizmoWidget() {}

void GizmoWidget::on_widget_render() {
    ImGuizmo::BeginFrame();
    ImGui::Begin(m_name.c_str());

    render_gizmo();

    ImGui::End();
}
void GizmoWidget::render_gizmo() {}

}  // namespace beet