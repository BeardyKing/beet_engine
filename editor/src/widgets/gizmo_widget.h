#include <beet/log.h>
#include <beet/types.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {

class GizmoWidget : public Widget {
   public:
    GizmoWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~GizmoWidget();
    void on_widget_render() override;

   private:
    void render_gizmo();
   private:

    EditorWidgets& m_editorWidgets;
};
}  // namespace beet