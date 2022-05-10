#pragma once

#include <beet/components.h>
#include <beet/log.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {

class InspectorWidget : public Widget {
   public:
    InspectorWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~InspectorWidget();
    void on_widget_render() override;

   private:
    void render_inspector();
    void render_name_component(components::Name& name);
    void render_transform_component(components::Transform& transform);
    void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

   private:
    EditorWidgets& m_editorWidgets;
    std::string m_handle;
    std::string m_uuid;
};

}  // namespace beet
