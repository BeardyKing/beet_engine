#pragma once

#include <beet/log.h>
#include <beet/types.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {
class TerminalWidget : public Widget {
   public:
    TerminalWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~TerminalWidget();
    void on_widget_render() override;

   private:
    void split();
    EditorWidgets& m_editorWidgets;

    std::ostringstream oss;
    std::vector<std::string> messages;
};

}  // namespace beet
