#include "terminal_widget.h"
#include <spdlog/sinks/ostream_sink.h>

namespace beet {
TerminalWidget::TerminalWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {
    // setup ostringstream and attach to spd log
    auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    log::default_logger()->sinks().push_back(ostream_sink);
    log::set_pattern("[%R:%S:%e] [%^%l%$] %v");
    log::warn("warning here");
    log::error("error here");
}

TerminalWidget::~TerminalWidget() {}

void TerminalWidget::on_widget_render() {
    ImGui::Begin(m_name.c_str());
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 indexLayoutSize = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH |
                                   ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Resizable;

    split();
    // TODO setup imgui clipper
    if (ImGui::BeginTable("Terminal", 3, flags)) {
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        size_t count{0};
        std::string start = "##";
        for (auto& msg : m_messages) {
            ImGui::TableNextRow();
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(187, 187, 187, 255));
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(msg.time.c_str());
                ImGui::PopStyleColor();
            }
            set_text_color(msg.level);
            {
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(msg.level.c_str());
            }
            {
                ImGui::TableSetColumnIndex(2);
                auto readFlags = ImGuiInputTextFlags_ReadOnly;
                std::string label = "##";
                label.append(std::to_string(count));
                ImGui::PushItemWidth(-1);
                ImGui::InputText(label.c_str(), msg.message.data(), msg.message.length(), readFlags);
                count++;
            }
            ImGui::PopStyleColor();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void TerminalWidget::split() {
    std::string str = oss.str();
    if (!str.empty()) {
        std::string delimiter = "\n";
        std::string delimiterTypes = " ";

        size_t pos = 0;
        std::string token;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            messages.emplace_back(token);
            str.erase(0, pos + delimiter.length());
        }

        messages.push_back(str);
        oss.str("");

        for (auto msg : messages) {
            TermMessage termMessage;
            int spaceCount{0};
            while ((pos = msg.find(delimiterTypes)) != std::string::npos) {
                token = msg.substr(0, pos);

                if (spaceCount == 0) {
                    termMessage.time = token;
                    msg.erase(0, pos + delimiterTypes.length());
                }
                if (spaceCount == 1) {
                    termMessage.level = token;
                    msg.erase(0, pos + delimiterTypes.length());
                }
                if (spaceCount == 2) {
                    termMessage.message = msg;
                    break;
                }
                spaceCount++;
            }
            m_messages.emplace_back(termMessage);
        }
    }
}

void TerminalWidget::set_text_color(const std::string& str) {
    std::string debug = "[debug]";
    std::string info = "[info]";
    std::string error = "[error]";
    std::string warn = "[warning]";

    if (str.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        return;
    }
    if (str[1] == 'd') {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(3, 157, 157, 255));
    } else if (str[1] == 'i') {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(93, 150, 44, 255));
    } else if (str[1] == 'e') {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(244, 61, 73, 255));
    } else if (str[1] == 'w') {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(229, 191, 1, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 255, 255));
    }

    //    if (str.find(debug)) {
    //        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(3, 157, 157, 255));
    //    } else if (str.find(info)) {
    //        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(93, 150, 44, 255));
    //    }
    //    if (str.find(warn)) {
    //        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(229, 191, 1, 255));
    //    }
    //    if (str.find(error)) {
    //        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(244, 61, 73, 255));
    //    }
    //    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
}
}  // namespace beet
