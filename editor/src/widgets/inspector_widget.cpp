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

    if (go.has_component<components::Tag>()) {
        render_tag_component(go.get_component<components::Tag>());
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

    if (go.has_component<components::PointLight>()) {
        render_point_light_component(go.get_component<components::PointLight>());
    }

    if (go.has_component<components::SkyBox>()) {
        render_skybox_component(go.get_component<components::SkyBox>());
    }
}

void InspectorWidget::render_name_component(components::Name& name) {
    char entityName[64]{};
    sprintf_s(entityName, "%s", name.name.c_str());

    ImGui::InputText("##INSPECTOR_NAME", entityName, IM_ARRAYSIZE(entityName));
    name.name = entityName;
    ImGui::Separator();
}

void InspectorWidget::render_tag_component(components::Tag& tag) {
    char tagName[64]{};
    sprintf_s(tagName, "%s", tag.get_tag().c_str());

    if (ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();
        ImGui::Columns(2, "tagName", false);
        ImGui::Text("Tag");
        ImGui::NextColumn();
        ImGui::InputText("##INSPECTOR_TAG", tagName, IM_ARRAYSIZE(tagName));
        tag.register_tag(tagName);
        ImGui::Unindent();
    }
}

void InspectorWidget::render_transform_component(components::Transform& transform) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();

        vec3 position = transform.get_position();
        vec3 rotation = transform.get_rotation_euler();
        vec3 scale = transform.get_scale();

        ImGui::Text("Position  ");
        ImGui::SameLine();
        ImGui::DragFloat3("##P", &position.x, -0.1f, 0.1f);

        ImGui::Text("Rotation  ");
        ImGui::SameLine();
        ImGui::DragFloat3("##R", &rotation.x, -1.0f, 1.0f);

        ImGui::Text("Scale     ");
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

        if (ImGui::TreeNode("Vertex Layout")) {
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
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Vertex Data")) {
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
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Index Data")) {
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
            ImGui::TreePop();
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
    auto albedoColor = material.get_albedo_color();
    auto textureTiling = material.get_texture_tiling();
    auto albedoScalar = material.get_albedo_scalar();
    auto normalScalar = material.get_normal_scalar();
    auto metallicScalar = material.get_metallic_scalar();
    auto roughnessScalar = material.get_roughness_scalar();
    auto occlusionScalar = material.get_occlusion_scalar();
    auto skyboxScalar = material.get_skybox_scalar();
    auto castShadows = material.get_cast_shadows();
    auto receivesShadows = material.get_receives_shadows();
    auto alphaCutoffEnabled = material.get_alpha_cutoff_enabled();
    auto alphaCutoffAmount = material.get_alpha_cutoff_amount();
    auto shader = material.get_shader();

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();

        ImGui::Columns(2, "shader", false);
        ImGui::Text("Shader");
        ImGui::NextColumn();

        // TODO currently only hard coded shaders can be bound to a material
        static int selectedShader = 0;
        std::string comboBoxContent;
        comboBoxContent.append(shader->get_asset_name());
        comboBoxContent.append("\0");

        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::Indent(style.FramePadding.x);

        ImGui::Combo("##shader", &selectedShader, shader->get_asset_name().c_str());
        ImGui::Unindent(style.FramePadding.x);
        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::SetNextItemOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
        if (ImGui::TreeNode("Surface inputs")) {
            ImGui::Columns(2, "surfaceInputsColumns", false);

            //--------------Albedo Map--------------//

            static uint16_t albedoMap_Image_Scalar = 1;
            if (ImGui::ImageButton((void*)material.get_texture(TextureType::Albedo)->get_texture_handle(),
                                   ImVec2(12.0f * albedoMap_Image_Scalar, 12.0f * albedoMap_Image_Scalar))) {
                albedoMap_Image_Scalar = (albedoMap_Image_Scalar == 1) ? 10 : 1;
            }
            ImGui::SameLine();
            ImGui::Selectable("Albedo map");
            ImGui::NextColumn();
            ImGui::SliderFloat("##albedoScalar", &albedoScalar, 0.3f, 10.0f);
            ImGui::SameLine();

            //--------------Color picker------------//
            if (ImGui::ColorButton("Albedo color",
                                   ImVec4(albedoColor.x, albedoColor.y, albedoColor.z, albedoColor.w))) {
                ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 320, ImGui::GetMousePos().y - 120));
                ImGui::OpenPopup("albedoColorPicker");
            }
            if (ImGui::BeginPopup("albedoColorPicker", ImGuiSelectableFlags_DontClosePopups)) {
                ImGui::ColorPicker4("##picker", &albedoColor.x, ImGuiColorEditFlags_None, NULL);
                ImGui::EndPopup();
            }
            ImGui::NextColumn();

            //--------------Normal Map--------------//

            static uint16_t normalMap_Image_Scalar = 1;
            if (ImGui::ImageButton((void*)material.get_texture(TextureType::Normal)->get_texture_handle(),
                                   ImVec2(12.0f * normalMap_Image_Scalar, 12.0f * normalMap_Image_Scalar))) {
                normalMap_Image_Scalar = (normalMap_Image_Scalar == 1) ? 10 : 1;
            }

            ImGui::SameLine();
            ImGui::Selectable("Normal map");
            ImGui::NextColumn();
            ImGui::SliderFloat("##normalScalar", &normalScalar, 0.3f, 15.0f);
            ImGui::NextColumn();

            //--------------Metallic Map--------------//
            static uint16_t metallicMap_Image_Scalar = 1;
            if (ImGui::ImageButton((void*)material.get_texture(TextureType::Metallic)->get_texture_handle(),
                                   ImVec2(12.0f * metallicMap_Image_Scalar, 12.0f * metallicMap_Image_Scalar))) {
                metallicMap_Image_Scalar = (metallicMap_Image_Scalar == 1) ? 10 : 1;
            }

            ImGui::SameLine();
            ImGui::Selectable("Metallic map");
            ImGui::NextColumn();
            ImGui::SliderFloat("##metallicScalar", &metallicScalar, 0.0f, 1.0f);
            ImGui::NextColumn();

            //--------------Roughness Map--------------//
            static uint16_t RoughnessMap_Image_Scalar = 1;
            if (ImGui::ImageButton((void*)material.get_texture(TextureType::Roughness)->get_texture_handle(),
                                   ImVec2(12.0f * RoughnessMap_Image_Scalar, 12.0f * RoughnessMap_Image_Scalar))) {
                RoughnessMap_Image_Scalar = (RoughnessMap_Image_Scalar == 1) ? 10 : 1;
            }

            ImGui::SameLine();
            ImGui::Selectable("Roughness map");

            ImGui::NextColumn();
            ImGui::SliderFloat("##toughnessScalar", &roughnessScalar, 0.0f, 4.0f);
            ImGui::NextColumn();

            //--------------Occlusion Map--------------//
            static uint16_t OcclusionMap_Image_Scalar = 1;
            if (ImGui::ImageButton((GLvoid*)material.get_texture(TextureType::Occlusion)->get_texture_handle(),
                                   ImVec2(12.0f * OcclusionMap_Image_Scalar, 12.0f * OcclusionMap_Image_Scalar))) {
                OcclusionMap_Image_Scalar = (OcclusionMap_Image_Scalar == 1) ? 10 : 1;
            }

            ImGui::SameLine();
            ImGui::Selectable("Occlusion map");

            ImGui::NextColumn();
            ImGui::SliderFloat("##cclusionScalar", &occlusionScalar, 0.0f, 10.0f);
            ImGui::NextColumn();

            ImGui::Columns(1);
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
        if (ImGui::TreeNode("Advanced options")) {
            ImGui::Columns(2, "advancedOptionsColumns", false);

            ImGui::Selectable("Texture tiling");
            ImGui::NextColumn();
            ImGui::DragFloat2("##textureTiling", &textureTiling.x, -0.0125f, 0.0125f);
            ImGui::NextColumn();

            ImGui::Selectable("Skybox scalar");
            ImGui::NextColumn();
            ImGui::SliderFloat("##skyboxScalar", &skyboxScalar, 0.0f, 2.0f);
            ImGui::NextColumn();

            ImGui::Selectable("Receives shadows");
            ImGui::NextColumn();
            ImGui::Checkbox("##Recieve shadows", &receivesShadows);
            ImGui::NextColumn();

            ImGui::Selectable("Alpha cutoff enabled");
            ImGui::NextColumn();
            ImGui::Checkbox("##alphaCutoffEnabled", &alphaCutoffEnabled);
            ImGui::NextColumn();

            ImGui::Selectable("Alpha cutoff amount");
            ImGui::NextColumn();
            ImGui::SliderFloat("##alphaCutoffAmount", &alphaCutoffAmount, 0.0f, 1.0f);
            ImGui::NextColumn();

            ImGui::TreePop();
        }
        ImGui::Columns(1);
        ImGui::Unindent();
    }
    ImGui::Separator();

    material.set_albedo_color(albedoColor);
    material.set_texture_tiling(textureTiling);
    material.set_albedo_scalar(albedoScalar);
    material.set_normal_scalar(normalScalar);
    material.set_metallic_scalar(metallicScalar);
    material.set_roughness_scalar(roughnessScalar);
    material.set_occlusion_scalar(occlusionScalar);
    material.set_skybox_scalar(skyboxScalar);
    material.set_cast_shadows(castShadows);
    material.set_receives_shadows(receivesShadows);
    material.set_alpha_cutoff_enabled(alphaCutoffEnabled);
    material.set_alpha_cutoff_amount(alphaCutoffAmount);
}

void InspectorWidget::render_point_light_component(components::PointLight& pointLight) {
    auto lightColor = pointLight.get_color();
    auto lightRange = pointLight.get_range();
    auto lightIntensity = pointLight.get_intensity();

    if (ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();
        ImGui::Columns(2, "pointLights", false);
        ImGui::Text("Light color");
        ImGui::NextColumn();

        if (ImGui::ColorButton("Color", ImVec4(lightColor.x, lightColor.y, lightColor.z, 1.0f))) {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 320, ImGui::GetMousePos().y - 120));
            ImGui::OpenPopup("lightColorPicker");
        }
        if (ImGui::BeginPopup("lightColorPicker", ImGuiSelectableFlags_DontClosePopups)) {
            ImGui::ColorPicker3("##lightPicker", &lightColor.x, ImGuiColorEditFlags_None);
            ImGui::EndPopup();
        }

        ImGui::NextColumn();
        ImGui::Text("Range");
        ImGui::NextColumn();
        ImGui::DragFloat("##lightRangeDF", &lightRange, 0.125f);

        ImGui::NextColumn();
        ImGui::Text("Intensity");
        ImGui::NextColumn();
        ImGui::DragFloat("##lightIntensityDF", &lightIntensity, 0.125f);

        ImGui::Unindent();
        ImGui::Columns(1);
    }
    ImGui::Separator();

    pointLight.set_color(lightColor);
    pointLight.set_range(lightRange);
    pointLight.set_intensity(lightIntensity);
}
void InspectorWidget::render_skybox_component(components::SkyBox& skyBox) {
    if (ImGui::CollapsingHeader("skybox", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
        ImGui::Indent();
        ImGui::SetNextItemOpen(true, ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::TreeNode("Skybox Textures - HDRi")) {
            ImGui::Spacing();
            ImGui::Columns(2, "surfaceInputsColumns", false);
            //=ALBEDO================
            ImGui::Selectable("Albedo map");
            ImGui::NextColumn();
            static uint16_t skyboxImageScalarAlbedo = 1;
            if (ImGui::ImageButton((void*)skyBox.get_texture(SkyBoxTextureType::SkyBox)->get_texture_handle(),
                                   ImVec2(12.0f * skyboxImageScalarAlbedo, 12.0f * skyboxImageScalarAlbedo))) {
                skyboxImageScalarAlbedo = (skyboxImageScalarAlbedo == 1) ? 10 : 1;
            }
            ImGui::NextColumn();
            //=IRRADIANCE================
            ImGui::Selectable("Irradiance map");
            ImGui::NextColumn();
            static uint16_t skyboxImageScalarIrradiance = 1;
            if (ImGui::ImageButton((void*)skyBox.get_texture(SkyBoxTextureType::Irradiance)->get_texture_handle(),
                                   ImVec2(12.0f * skyboxImageScalarIrradiance, 12.0f * skyboxImageScalarIrradiance))) {
                skyboxImageScalarIrradiance = (skyboxImageScalarIrradiance == 1) ? 10 : 1;
            }
            ImGui::NextColumn();

            //=RADIANCE================
            ImGui::Selectable("Radiance map");
            ImGui::NextColumn();
            static uint16_t skyboxImageScalarRadiance = 1;
            if (ImGui::ImageButton((void*)skyBox.get_texture(SkyBoxTextureType::Radiance)->get_texture_handle(),
                                   ImVec2(12.0f * skyboxImageScalarRadiance, 12.0f * skyboxImageScalarRadiance))) {
                skyboxImageScalarRadiance = (skyboxImageScalarRadiance == 1) ? 10 : 1;
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }
        ImGui::Unindent();
    }
}

InspectorWidget::~InspectorWidget() {
    log::debug("InspectorWidget destroyed");
}

}  // namespace beet
