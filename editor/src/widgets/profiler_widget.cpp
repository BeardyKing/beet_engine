#include "profiler_widget.h"

namespace beet {

ProfilerWidget::ProfilerWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

void ProfilerWidget::on_widget_render() {
    std::array<int32_t, (size_t)FrameBufferType::LAST> timings =
        m_editorWidgets.get_engine().get_renderer_module().lock()->get_render_pass_timings();
    ImGui::Begin(m_name.c_str());

    ImGui::Text("%s", "Render pass timings");
    ImGui::Spacing();

    for (int i = 0; i < (size_t)FrameBufferType::LAST; ++i) {
        if (m_recentTimings[i].size() >= dequeLength) {
            m_recentTimings[i].pop_front();
        }

        float currentTiming = (float)timings[i] > FLT_EPSILON * 10 ? (float)timings[i] / 1000000.0f : 0.0f;
        m_recentTimings[i].push_back(currentTiming);

        if (currentTiming > m_largestValueSeen[i]) {
            m_largestValueSeen[i] = currentTiming;
        }

        const std::vector<float> tmp{m_recentTimings[i].begin(), m_recentTimings[i].end()};
        ImVec2 size = ImGui::GetContentRegionAvail();
        size.y = 30.0f;
        ImGui::Text("%s - (%.2g ms)", renderPassNames[i], tmp.back());
        ImGui::PlotLines("##", tmp.data(), tmp.size(), 0, NULL, 0.0f, m_largestValueSeen[i], size);
    }
    
    ImGui::End();
}

ProfilerWidget::~ProfilerWidget() {
    log::debug("ProfilerWidget destroyed");
}

}  // namespace beet
