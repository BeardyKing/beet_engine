#pragma once

#include <beet/components.h>
#include <beet/log.h>
#include "editor_widgets.h"
#include "widget.h"

namespace beet {

class InspectorWidget : public Widget {
   public:
    InspectorWidget(const std::string& name, EditorWidgets& editorWidgets);
    ~InspectorWidget();
    void on_widget_render() override;

   private:
    void render_inspector();

    void render_name_component(components::Name& name);
    void render_tag_component(components::Tag& tag);
    void render_transform_component(components::Transform& transform);
    void render_mesh_component(components::InstanceMesh& mesh);
    void render_camera_component(components::Camera& camera);
    void render_material_component(components::Material& material);
    void render_point_light_component(components::PointLight& pointLight);
    void render_skybox_component(components::SkyBox& skyBox);

   private:
    EditorWidgets& m_editorWidgets;
    std::string m_handle;
    std::string m_uuid;
};

}  // namespace beet
