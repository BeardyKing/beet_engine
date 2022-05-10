#pragma once

#include <imgui.h>
#include <string>

namespace beet {

class Widget {
   public:
    explicit Widget(const std::string& name) {}

    virtual void on_awake() {}
    virtual void on_widget_render() {}
    virtual void on_destroy() {}

   private:
    std::string m_name = "widget";
};
}  // namespace beet
