#include "editor_widgets.h"

#include "widgets/demo_widget.h"

namespace beet {
EditorWidgets::EditorWidgets(Engine& engine) : m_engine(engine) {
    add_widget(std::make_shared<DemoWidget>("Demo widget"));
}

void EditorWidgets::add_widget(std::shared_ptr<Widget> widget) {
    m_widgets.emplace_back(widget);
}

void EditorWidgets::on_awake() {
    auto window = m_engine.get_window_module().lock()->get_glfw_window();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}
void EditorWidgets::on_update(double m_delta_time) {}
void EditorWidgets::on_late_update() {
    ImGuiIO& io = ImGui::GetIO();

    bool show_demo_window = true;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto& widget : m_widgets) {
        widget->on_widget_render();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
void EditorWidgets::on_destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

}  // namespace beet