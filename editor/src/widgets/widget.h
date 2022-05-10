#pragma once

#include <imgui.h>
#include <string>

namespace beet {

class Widget {
   public:
    explicit Widget(const std::string& name) : m_name(name) {}

    virtual void on_awake() {}
    virtual void on_widget_render() {}
    virtual void on_destroy() {}

   protected:
    std::string m_name = {"widget"};
    bool m_isActive{true};
};
}  // namespace beet
