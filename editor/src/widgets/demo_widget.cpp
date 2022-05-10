#include "demo_widget.h"

beet::DemoWidget::DemoWidget(const std::string& name) : Widget(name) {}

void beet::DemoWidget::on_widget_render() {
    if (m_isActive) {
        ImGui::ShowDemoWindow(&m_isActive);
    }
}

beet::DemoWidget::~DemoWidget() {
    log::debug("DemoWidget destroyed");
}
