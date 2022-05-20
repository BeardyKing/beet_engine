#include "terminal_widget.h"
#include <spdlog/sinks/ostream_sink.h>

namespace beet {
TerminalWidget::TerminalWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {
    // setup ostringstream and attach to spd log
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(m_oss);
    log::default_logger()->sinks().push_back(ostream_sink);
    log::set_pattern("[%R:%S:%e] [%^%l%$] %v");
    m_terminalMessages.reserve(sizeof(TermMessage) * 20000);
}

TerminalWidget::~TerminalWidget() {}

void TerminalWidget::on_widget_render() {
    ImGui::Begin(m_name.c_str());
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH |
                                   ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Resizable;
    static ImGuiInputTextFlags readFlags = ImGuiInputTextFlags_ReadOnly;

    split();

    // TODO setup imgui clipper
    if (ImGui::BeginTable("Terminal", 3, flags)) {
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        size_t count{0};
        ImGuiListClipper clipper;
        clipper.Begin(m_terminalMessages.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(0.0f);

                ImGui::TableNextRow();
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(187, 187, 187, 255));
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", m_terminalMessages[row].time.c_str());
                    ImGui::PopStyleColor();
                }
                set_text_color(m_terminalMessages[row].level);
                {
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", m_terminalMessages[row].level.c_str());
                }
                {
                    ImGui::TableSetColumnIndex(2);
                    std::string label = "##";
                    label.append(std::to_string(count));
                    ImGui::PushItemWidth(-1);
                    ImGui::InputText(label.c_str(), m_terminalMessages[row].message.data(),
                                     m_terminalMessages[row].message.length(), readFlags);
                    count++;
                }
                ImGui::PopStyleColor();
            }
        }

        ImGui::EndTable();

        // workaround check if logs have been made since last frame
        // ImGui locked scrolling spacing so last log can be seen not needed when is not scrolling locked to bottom
        if (m_lastAmountOfLogs != m_terminalMessages.size()) {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            m_lastAmountOfLogs = m_terminalMessages.size();
        }
    }
    ImGui::End();
}

void TerminalWidget::split() {
    std::string str = m_oss.str();
    if (!str.empty()) {
        size_t pos = 0;
        while ((pos = str.find("\n")) != std::string::npos) {
            m_tempToken = str.substr(0, pos);
            m_rawMessages.emplace_back(m_tempToken);
            str.erase(0, pos + 1);
        }

        m_oss.str("");

        int spaceCount{0};
        for (auto& msg : m_rawMessages) {
            spaceCount = 0;
            while ((pos = msg.find(" ")) != std::string::npos) {
                m_tempToken = msg.substr(0, pos);

                if (spaceCount == 0) {
                    m_tempTermMessage.time = m_tempToken;
                    msg.erase(0, pos + 1);
                } else if (spaceCount == 1) {
                    m_tempTermMessage.level = m_tempToken;
                    msg.erase(0, pos + 1);
                } else {
                    break;
                }
                m_tempTermMessage.message = msg;
                spaceCount++;
            }
            m_terminalMessages.emplace_back(m_tempTermMessage);
        }
    }
    m_rawMessages.clear();
}

void TerminalWidget::set_text_color(const std::string& str) {
    if (str.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        return;
    }
    if (str[1] == 'd') {  // debug
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(3, 157, 157, 255));
    } else if (str[1] == 'i') {  // info
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(93, 150, 44, 255));
    } else if (str[1] == 'e') {  // error
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(244, 61, 73, 255));
    } else if (str[1] == 'w') {  // warning
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(229, 191, 1, 255));
    } else {  // fallback
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 255, 255));
    }
}
}  // namespace beet
