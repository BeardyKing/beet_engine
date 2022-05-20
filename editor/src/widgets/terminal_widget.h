#pragma once

#include <beet/log.h>
#include <beet/types.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {

struct TermMessage {
    std::string time;
    std::string level;
    std::string message;
};

class TerminalWidget : public Widget {
   public:
    TerminalWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~TerminalWidget();
    void on_widget_render() override;

   private:
    void set_text_color(const std::string& str);
    void split();
    EditorWidgets& m_editorWidgets;

    std::ostringstream oss;
    std::vector<std::string> messages;
    std::vector<std::string> test_messages;
    std::vector<TermMessage> m_messages;
};

}  // namespace beet
