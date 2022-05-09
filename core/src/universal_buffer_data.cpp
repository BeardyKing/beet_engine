#include <beet/log.h>
#include <beet/universal_buffer_data.h>

namespace beet {

void UniversalBufferData::init() {
    init_matrix_ubo();
    init_point_light_ubo();
}

void UniversalBufferData::init_matrix_ubo() {
    const size_t uboMatrixSize = sizeof(mat4) * 2 + sizeof(vec4);

    glGenBuffers(1, &m_matrixDataHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matrixDataHandle);
    glBufferData(GL_UNIFORM_BUFFER, uboMatrixSize, nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_matrixDataHandle, 0, uboMatrixSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniversalBufferData::init_point_light_ubo() {
    const size_t uboPointSize = (sizeof(PackedPointLightData) * MAX_POINT_LIGHTS) + sizeof(vec4);

    glGenBuffers(1, &m_pointLightDataHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightDataHandle);
    glBufferData(GL_UNIFORM_BUFFER, uboPointSize, nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_pointLightDataHandle, 0, uboPointSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniversalBufferData::update_point_light_data(const std::vector<PackedPointLightData>& pointLightData) {
    size_t amountOfActiveLights = pointLightData.size();
    
    std::array<PackedPointLightData, MAX_POINT_LIGHTS> lightData{};
    std::copy_n(pointLightData.begin(),
                (pointLightData.size() > MAX_POINT_LIGHTS) ? MAX_POINT_LIGHTS : pointLightData.size(),
                lightData.begin());

    glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightDataHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(amountOfActiveLights, 0, 0, 0)));

    size_t stride{0};
    for (auto& light : lightData) {
        glBufferSubData(GL_UNIFORM_BUFFER, stride + sizeof(vec4), sizeof(vec4),
                        value_ptr(light.pointPosition_pointRange));
        glBufferSubData(GL_UNIFORM_BUFFER, stride + sizeof(vec4) + sizeof(vec4) * MAX_POINT_LIGHTS, sizeof(vec4),
                        value_ptr(light.pointColor_pointIntensity));
        stride += sizeof(vec4);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniversalBufferData::update_view_projection_data(const mat4& view, const mat4& projection, const vec3& viewPos) {
    glBindBuffer(GL_UNIFORM_BUFFER, m_matrixDataHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4) * 2, sizeof(vec4), value_ptr(vec4(viewPos, 0)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

}  // namespace beet
