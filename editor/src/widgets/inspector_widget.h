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
    void render_name_component(components::Name& name);

   private:
    EditorWidgets& m_editorWidgets;
    std::string m_handle;
    std::string m_uuid;
};

}  // namespace beet
