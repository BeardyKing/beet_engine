#include "terminal_widget.h"
#include <spdlog/sinks/ostream_sink.h>

namespace beet {
TerminalWidget::TerminalWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {
    // setup ostringstream and attach to spd log
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    log::default_logger()->sinks().push_back(ostream_sink);
}

TerminalWidget::~TerminalWidget() {}

void TerminalWidget::on_widget_render() {
    ImGui::Begin(m_name.c_str());

    split();

    for (auto& msg : messages) {
        // TODO setup imgui push text color based on info, debug, error;

        ImGui::Text(msg.c_str());
    }

    ImGui::End();
}
void TerminalWidget::split() {
    // TODO consider splitting into [time][warning level][message] as struct/class
    std::string str = oss.str();
    if (!str.empty()) {
        std::string delimiter = "\n";

        size_t pos = 0;
        std::string token;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            messages.emplace_back(token);
            str.erase(0, pos + delimiter.length());
        }

        messages.push_back(str);
        oss.str("");
    }
}
}  // namespace beet
