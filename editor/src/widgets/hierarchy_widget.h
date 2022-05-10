

#pragma once

#include <beet/log.h>
#include "widget.h"

namespace beet {

class HierarchyWidget : public Widget {
   public:
    HierarchyWidget(const std::string& name);
    ~HierarchyWidget();
    void on_widget_render() override;

   private:
    bool m_isActive{true};
};

}  // namespace beet
