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
        auto revealTexture = fbo.get_reveal_texture();

        std::string name = fbo.get_name();
        auto rawSize = fbo.get_size();
        float aspectX = (float)rawSize.y / (float)rawSize.x;
        float aspectY = (float)rawSize.x / (float)rawSize.y;

        float width = ImGui::GetWindowContentRegionWidth();
        float height = width * aspectX;

        ImVec2 imageSize(width, height);

        auto flags = ImGuiTreeNodeFlags_AllowItemOverlap;
        ImVec2 uv_min = ImVec2(0.0f, 0.0f);
        ImVec2 uv_max = ImVec2(1.0f, -1.0f);

        if (colorTexture) {
            if (ImGui::CollapsingHeader((name + " : color texture").c_str(), flags)) {
                ImGui::Indent();
                ImGui::Text("%s", ("size : " + to_string(rawSize)).c_str());
                ImGui::Text("%s", ("aspect ratio : " + std::to_string(aspectY)).c_str());
                ImGui::Unindent();
                ImGui::Image((GLuint*)fbo.get_color_texture(), imageSize, uv_min, uv_max);
            }
        }

        if (depthTexture) {
            if (ImGui::CollapsingHeader((name + " : depth texture").c_str(), flags)) {
                ImGui::Indent();
                ImGui::Text("%s", ("size : " + to_string(rawSize)).c_str());
                ImGui::Text("%s", ("aspect : " + std::to_string(aspectY)).c_str());
                ImGui::Unindent();
                ImGui::Image((GLuint*)fbo.get_depth_texture(), imageSize, uv_min, uv_max);
            }
        }
        if (revealTexture) {
            if (ImGui::CollapsingHeader((name + " : reveal texture").c_str(), flags)) {
                ImGui::Indent();
                ImGui::Text("%s", ("size : " + to_string(rawSize)).c_str());
                ImGui::Text("%s", ("aspect : " + std::to_string(aspectY)).c_str());
                ImGui::Unindent();
                ImGui::Image((GLuint*)fbo.get_reveal_texture(), imageSize, uv_min, uv_max);
            }
        }
    }
}

}  // namespace beet