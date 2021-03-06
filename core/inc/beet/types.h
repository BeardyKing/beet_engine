#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <uuid.h>

namespace beet {

namespace asset {

enum class AssetState { Idle, Loading, Finished, Failed, LAST };
enum class AssetType { Unknown, Texture, Mesh, Shader, Cubemap, LAST };
enum class TextureType { Albedo, Normal, Metallic, Roughness, Occlusion, LAST };

}  // namespace asset
using namespace glm;
using namespace uuids;

enum class FrameBufferType : uint16_t {
    Back,
    Depth,
    ObjectPicking,
    Opaque,
    Transparency,
    PostProcess,
    ShadowOne,
    ShadowTwo,
    ShadowThree,
    ShadowFour,
    LAST
};

enum class SkyBoxTextureType { SkyBox, Radiance, Irradiance, LAST };

struct PackedPointLightData {
    vec4 pointPosition_pointRange;
    vec4 pointColor_pointIntensity;
};

class VertexLayout {
   public:
    VertexLayout(vec3 position, vec3 normal, vec2 textureCoordinate, vec3 tangent)
        : m_position(position), m_normal(normal), m_uv(textureCoordinate), m_tangent(tangent) {}
    vec3 m_position;
    vec3 m_normal;
    vec2 m_uv;
    vec3 m_tangent;
};

using vec2i = vec<2, int>;
using vec2u = vec<2, uint>;
using vec2d = vec<2, double>;
using vec3i = vec<3, int>;
using vec3d = vec<3, double>;
using vec4i = vec<4, int>;
using vec4d = vec<4, double>;
}  // namespace beet
