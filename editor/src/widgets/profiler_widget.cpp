#include "profiler_widget.h"

namespace beet {

ProfilerWidget::ProfilerWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

void ProfilerWidget::on_widget_render() {
    std::array<int32_t, (size_t)FrameBufferType::LAST> timings =
        m_editorWidgets.get_engine().get_renderer_module().lock()->get_render_pass_timings();
    ImGui::Begin(m_name.c_str());

    ImGui::Text("%s", "CPU: system timings");
    ImGui::Spacing();
    ImGui::Spacing();
    {
        double lastTime = m_editorWidgets.get_engine().get_window_module().lock()->get_delta_time();
        if (m_recentCPUTimings.size() >= dequeLength) {
            m_recentCPUTimings.pop_back();
        }

        float currentTiming = (float)lastTime * 1000.0f;
        m_recentCPUTimings.push_front(currentTiming);

        float tmp[dequeLength]{};
        int idx{dequeLength - 1};
        for (auto& item : m_recentCPUTimings) {
            tmp[idx] = item;
            idx--;
        }

        ImVec2 size = ImGui::GetContentRegionAvail();
        size.y = 35.0f;
        ImGui::Text("%s - (%.2f ms)", "Last delta time", currentTiming);
        ImGui::PlotLines("##", tmp, IM_ARRAYSIZE(tmp), 0, NULL, 0.0f, 20.0f, size);
    }
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    {
        ImGui::Text("%s", "GPU: Render pass timings");
        ImGui::Spacing();
        ImGui::Spacing();
        for (int i = 0; i < (size_t)FrameBufferType::LAST; ++i) {
            if (m_recentGPUTimings[i].size() >= dequeLength) {
                m_recentGPUTimings[i].pop_back();
            }

            float currentTiming = (float)timings[i] > FLT_EPSILON * 10 ? (float)timings[i] / 1000000.0f : 0.0f;
            m_recentGPUTimings[i].push_front(currentTiming);

            if (currentTiming > m_largestGPUValuesSeen[i]) {
                m_largestGPUValuesSeen[i] = currentTiming;
            }

            float tmp[dequeLength]{};
            int idx{dequeLength - 1};
            for (auto& item : m_recentGPUTimings[i]) {
                tmp[idx] = item;
                idx--;
            }

            ImVec2 size = ImGui::GetContentRegionAvail();
            size.y = 35.0f;
            ImGui::Text("%s - (%.2g ms)", renderPassNames[i], currentTiming);
            ImGui::PlotLines("##", tmp, IM_ARRAYSIZE(tmp), 0, NULL, 0.0f, m_largestGPUValuesSeen[i], size);
        }
    }
    ImGui::End();
}

ProfilerWidget::~ProfilerWidget() {
    log::debug("ProfilerWidget destroyed");
}

}  // namespace beet
