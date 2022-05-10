#include "hierarchy_widget.h"

namespace beet {

HierarchyWidget::HierarchyWidget(const std::string& name) : Widget(name) {}

void HierarchyWidget::on_widget_render() {}

HierarchyWidget::~HierarchyWidget() {
    log::debug("HierarchyWidget destroyed");
}

}  // namespace beet
