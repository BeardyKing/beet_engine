#pragma once

#include "glad/glad.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __linux__
#define GLFW_EXPOSE_NATIVE_X11
#elif __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "beet/engine.h"
#include "beet/log.h"
#include "beet/subsystem.h"

#include "widget.h"

#include <memory>
#include <vector>

namespace beet {
class EditorWidgets : public Subsystem {
   public:
    explicit EditorWidgets(Engine& engine);
    void add_widget(std::shared_ptr<Widget> widget);

    void on_awake() override;
    void on_update(double m_delta_time) override;
    void on_late_update() override;
    void on_destroy() override;
    uuids::uuid get_selected_entity() { return m_selectedEntity; };
    void set_selected_entity(uuids::uuid selectedEntity) { m_selectedEntity = selectedEntity; };

   private:
    std::vector<std::shared_ptr<Widget>> m_widgets;
    Engine& m_engine;
    double m_deltaTime = 0.01;
    uuids::uuid m_selectedEntity{};
    void set_theme();

   public:
};

}  // namespace beet
