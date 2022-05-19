#include "framebuffer_widget.h"
#include "beet/window.h"

namespace beet {

FramebufferWidget::FramebufferWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

FramebufferWidget::~FramebufferWidget() {}

void FramebufferWidget::on_widget_render() {
    ImGui::Begin(m_name.c_str());
    render_frame_buffers();
    ImGui::End();
}
void FramebufferWidget::render_frame_buffers() {
    auto fbm = m_editorWidgets.get_engine().get_framebuffer_module().lock();
    auto fbos = fbm->get_all_framebuffers();

    for (auto& fbo : fbos) {
        if (fbo.get_framebuffer() == 0) {
            continue;
        }

        auto colorTexture = fbo.get_color_texture();
        auto depthTexture = fbo.get_depth_texture();

        std::string name = fbo.get_name();
        auto rawSize = fbo.get_size();
        float aspectX = (float)rawSize.y / (float)rawSize.x;

        float width = ImGui::GetWindowContentRegionWidth();
        float height = width * aspectX;

        ImVec2 imageSize(width, height);

        auto flags = ImGuiTreeNodeFlags_AllowItemOverlap;

        if (colorTexture) {
            if (ImGui::CollapsingHeader((name + " : color texture").c_str(), flags)) {
                ImGui::Image((GLuint*)fbo.get_color_texture(), imageSize);
            }
        }

        if (depthTexture) {
            if (ImGui::CollapsingHeader((name + " : depth texture").c_str(), flags)) {
                ImGui::Image((GLuint*)fbo.get_depth_texture(), imageSize);
            }
        }
    }
}

}  // namespace beet