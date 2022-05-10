#pragma once

#include <beet/log.h>
#include "widget.h"

namespace beet {

class DemoWidget : public Widget {
   public:
    DemoWidget(const std::string& name);
    ~DemoWidget();
    void on_widget_render() override;

   private:
    bool m_isActive{true};
};

}  // namespace beet