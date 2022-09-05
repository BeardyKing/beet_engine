#pragma once

#include <deque>
#include "beet/log.h"
#include "editor_widgets.h"
#include "widget.h"

namespace beet {

constexpr uint32_t dequeLength = 144 * 3;
constexpr std::array<const char*, (size_t)FrameBufferType::LAST> renderPassNames{
    "Back",        "Depth",     "ObjectPicking", "Opaque",      "Transparency",
    "PostProcess", "ShadowOne", "ShadowTwo",     "ShadowThree", "ShadowFour",
};

class ProfilerWidget : public Widget {
   public:
    ProfilerWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~ProfilerWidget();
    void on_widget_render() override;

   private:
    EditorWidgets& m_editorWidgets;
    std::array<std::deque<float>, (size_t)FrameBufferType::LAST> m_recentGPUTimings;
    std::array<float, (size_t)FrameBufferType::LAST> m_largestGPUValuesSeen{};

    std::deque<float> m_recentCPUTimings{};
};

}  // namespace beet