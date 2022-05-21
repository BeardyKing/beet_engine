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
    void scroll_to_bottom();
    EditorWidgets& m_editorWidgets;

    std::ostringstream m_oss;
    // TODO consider using large array or circular buffer
    std::vector<TermMessage> m_terminalMessages;

   private:
    std::vector<std::string> m_rawMessages;
    TermMessage m_tempTermMessage;
    std::string m_tempToken{};
    int m_lastAmountOfLogs{0};
    bool m_setScrollBottom = false;
};

}  // namespace beet
