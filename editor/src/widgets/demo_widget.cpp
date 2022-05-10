#include "demo_widget.h"

namespace beet {

DemoWidget::DemoWidget(const std::string& name) : Widget(name) {}

void DemoWidget::on_widget_render() {
    if (m_isActive) {
        ImGui::ShowDemoWindow(&m_isActive);
    }
}

DemoWidget::~DemoWidget() {
    log::debug("DemoWidget destroyed");
}

}  // namespace beet
