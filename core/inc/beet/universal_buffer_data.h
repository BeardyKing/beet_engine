#pragma once
#include <beet/types.h>
#include <glad/glad.h>
#include <vector>

namespace beet {

static constexpr uint16_t MAX_POINT_LIGHTS = 128;

class UniversalBufferData {
   public:
    UniversalBufferData() = default;
    void init();
    void update_point_light_data(const std::vector<PackedPointLightData>& pointLightData);
    void update_view_projection_data(const mat4& view, const mat4& projection, const vec3& viewPos);

   private:
    void init_matrix_ubo();
    void init_point_light_ubo();

   private:
    GLuint m_matrixDataHandle{0};
    GLuint m_pointLightDataHandle{0};
};

}  // namespace beet
