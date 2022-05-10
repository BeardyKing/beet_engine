#pragma once

#include <beet/components.h>
#include <beet/log.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {

class HierarchyWidget : public Widget {
   public:
    HierarchyWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~HierarchyWidget();
    void on_widget_render() override;

   private:
    EditorWidgets& m_editorWidgets;
};

}  // namespace beet
