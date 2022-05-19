#pragma once

#include <beet/log.h>
#include <beet/types.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {
class FramebufferWidget : public Widget {
   public:
    FramebufferWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~FramebufferWidget();
    void on_widget_render() override;

   private:
    void render_frame_buffers();

   private:
    EditorWidgets& m_editorWidgets;
};

}  // namespace beet
