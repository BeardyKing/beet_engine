#pragma once

namespace beet {
namespace asset {

enum class AssetState { Idle, Loading, Finished, Failed, LAST };
enum class AssetType { Unknown, Texture, Mesh, Shader, Cubemap, LAST };
enum class TextureType { Albedo, Normal, Metallic, Roughness, Occlusion, LAST };

}  // namespace asset
}  // namespace beet