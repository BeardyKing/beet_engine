#include <beet/log.h>
#include <beet/types.h>
#include "editor_widgets.h"
#include "widget.h"

#include <ImGuizmo.h>

namespace beet {

struct PixelInfo {
    float ObjectID;
    float DrawID;
    float PrimID;

    PixelInfo() {
        ObjectID = 0.0f;
        DrawID = 0.0f;
        PrimID = 0.0f;
    };
};

class GizmoWidget : public Widget {
   public:
    GizmoWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~GizmoWidget();

    void on_widget_render() override;

   private:
    void mouse_picking();
    void render_editor_scene(Framebuffer& fbo);
    void render_gizmo(const ImVec2& windowPos, const ImVec2& imageSize);

   private:
    EditorWidgets& m_editorWidgets;

   private:
    ImGuizmo::OPERATION m_currentGizmoOperation{ImGuizmo::OPERATION::TRANSLATE};
    ImGuizmo::MODE m_currentGizmoMode{ImGuizmo::MODE::LOCAL};
};
}  // namespace beet