#include "inspector_widget.h"
#include <beet/scene.h>
#include <imgui.h>

namespace beet {

InspectorWidget::InspectorWidget(const std::string& name, EditorWidgets& editorWidgets)
    : Widget(name), m_editorWidgets(editorWidgets) {}

void InspectorWidget::on_widget_render() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(m_name.c_str());

    render_inspector();

    ImGui::End();
}

void InspectorWidget::render_inspector() {
    using namespace components;
    auto selected = m_editorWidgets.get_selected_entity();

    auto sceneOpt = Scene::get_active_scene();
    if (!sceneOpt) {
        return;
    }

    Scene& scene = sceneOpt.value();
    entt::registry& registry = scene.get_registry();

    auto goOpt = scene.get_game_object_from_id(selected);
    if (!goOpt) {
        return;
    }

    GameObject go = goOpt.value();

    m_handle = std::to_string(static_cast<size_t>(go.get_handle()));
    m_uuid = uuids::to_string(go.get_id());

    //=Render Inspector===========

    if (go.has_component<components::Name>()) {
        render_name_component(go.get_component<components::Name>());
    }

    if (go.has_component<components::Transform>()) {
        render_transform_component(go.get_component<components::Transform>());
    }

    if (go.has_component<components::Camera>()) {
        render_camera_component(go.get_component<components::Camera>());
    }

    if (go.has_component<components::InstanceMesh>()) {
        render_mesh_component(go.get_component<components::InstanceMesh>());
    }

    if (go.has_component<components::Material>()) {
        render_material_component(go.get_component<components::Material>());
    }
}

void InspectorWidget::render_name_component(components::Name& name) {
    char entityName[64]{};
    sprintf_s(entityName, "%s", name.name.c_str());

    ImGui::InputText("##INSPECTOR_NAME", entityName, IM_ARRAYSIZE(entityName));
    name.name = entityName;
    ImGui::Separator();
}

void InspectorWidget::render_transform_component(components::Transform& transform) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();

        vec3 position = transform.get_position();
        vec3 rotation = transform.get_rotation_euler();
        vec3 scale = transform.get_scale();

        ImGui::Text("Position : ");
        ImGui::SameLine();
        ImGui::DragFloat3("##P", &position.x, -0.1f, 0.1f);

        ImGui::Text("Rotation : ");
        ImGui::SameLine();
        ImGui::DragFloat3("##R", &rotation.x, -1.0f, 1.0f);

        ImGui::Text("Scale :    ");
        ImGui::SameLine();
        ImGui::DragFloat3("##S", &scale.x, -0.1f, 0.1f);

        transform.set_position(position);
        transform.set_rotation_euler(rotation);
        transform.set_scale(scale);
        ImGui::Unindent();
    }
    ImGui::Separator();
}
void InspectorWidget::render_mesh_component(components::InstanceMesh& instanceMesh) {
    auto& mesh = *instanceMesh.get_mesh().lock();
    auto mIndices = mesh.get_index_buffer();
    auto indices = mesh.get_indices();
    auto vertexLayout = mesh.get_vertex_layout();
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_RowBg |
                                   ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

    ImVec2 vertexLayoutSize = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5);
    ImVec2 indexLayoutSize = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
    ImVec2 vertLayoutSize = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);

    if (ImGui::CollapsingHeader("Instance Mesh",
                                ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();
        if (ImGui::BeginTable("Mesh general information", 2, flags, ImVec2(0, TEXT_BASE_HEIGHT * 4))) {
            ImGui::TableSetupColumn("title", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("data", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", "Vertex size");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%llu", sizeof(VertexLayout));

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", "Mesh name");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", mesh.get_asset_name().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", "Mesh path");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", mesh.get_full_path().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", "Asset state");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%i", mesh.get_asset_state());

            ImGui::EndTable();
        }

        if (ImGui::CollapsingHeader("Vertex Layout", ImGuiTreeNodeFlags_AllowItemOverlap)) {
            if (ImGui::BeginTable("VertexLayout", 3, flags, vertexLayoutSize)) {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Stride", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                uint32_t stride{0};
                uint32_t size{0};
                {
                    size = sizeof(VertexLayout::m_position);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", "Position");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%i", stride);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%i", size);
                    stride += size;
                }
                {
                    size = sizeof(VertexLayout::m_normal);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", "Normal");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%i", stride);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%i", size);
                    stride += size;
                }
                {
                    size = sizeof(VertexLayout::m_uv);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", "UV");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%i", stride);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%i", size);
                    stride += size;
                }
                {
                    size = sizeof(VertexLayout::m_tangent);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", "Tangent");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%i", stride);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%i", size);
                    stride += size;
                }
                ImGui::EndTable();
            }
        }

        if (ImGui::CollapsingHeader("Vertex Data", ImGuiTreeNodeFlags_AllowItemOverlap)) {
            if (ImGui::BeginTable("VertexData", 5, flags, indexLayoutSize)) {
                ImGui::TableSetupColumn("index", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("UV", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Tangent", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                ImGuiListClipper clipper;
                clipper.Begin(vertexLayout.size() - 4);
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("\n%i", row);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("x: %f \ny: %f \nz: %f", vertexLayout[row].m_position.x,
                                    vertexLayout[row].m_position.y, vertexLayout[row].m_position.z);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("x: %f \ny: %f \nz: %f", vertexLayout[row].m_normal.x, vertexLayout[row].m_normal.y,
                                    vertexLayout[row].m_normal.z);
                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("u: %f \nv: %f", vertexLayout[row].m_uv.x, vertexLayout[row].m_uv.y);
                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("x: %f \ny: %f \nz: %f", vertexLayout[row].m_tangent.x,
                                    vertexLayout[row].m_tangent.y, vertexLayout[row].m_tangent.z);
                    }
                }
                ImGui::EndTable();
            }
        }

        if (ImGui::CollapsingHeader("Index Data", ImGuiTreeNodeFlags_AllowItemOverlap)) {
            if (ImGui::BeginTable("IndexData", 4, flags, indexLayoutSize)) {
                ImGui::TableSetupColumn("index", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                ImGuiListClipper clipper;
                clipper.Begin(indices.size() - 3);
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%i", row);
                        for (int column = 0; column < 3; ++column) {
                            ImGui::TableSetColumnIndex(column + 1);
                            ImGui::Text("%i", indices[row + column]);
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::Unindent();
    }
    ImGui::Separator();
}
void InspectorWidget::render_camera_component(components::Camera& camera) {
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();
        float fov = degrees(camera.get_fov());
        auto zNear = camera.get_z_near();
        auto zFar = camera.get_z_far();
        auto lookTarget = camera.get_look_target();

        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("VertexLayout", 2, flags, ImVec2(0, TEXT_BASE_HEIGHT * 4), 150.0f)) {
            ImGui::TableSetupColumn("Type", ImGuiTableFlags_SizingStretchProp);
            ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", "FOV");

                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat("##fovY", &fov, 0.125f);
            }

            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", "Clipping");
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", "Near");
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat("##zNearPlane", &zNear, 0.125f);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", "Far");
                ImGui::TableSetColumnIndex(1);
                ImGui::DragFloat("##zFarPlane", &zFar, 0.125f);
                ImGui::TableNextRow();
            }
            {
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", "Look target");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", to_string(lookTarget).c_str());
                ImGui::TableNextRow();
            }

            ImGui::EndTable();
        }

        camera.set_fov(fov);
        camera.set_z_near(zNear);
        camera.set_z_far(zFar);

        ImGui::Unindent();
    }
    ImGui::Separator();
}

void InspectorWidget::render_material_component(components::Material& material) {
    ImGui::Text("Material");
    ImGui::Separator();
}

InspectorWidget::~InspectorWidget() {
    log::debug("InspectorWidget destroyed");
}

}  // namespace beet
