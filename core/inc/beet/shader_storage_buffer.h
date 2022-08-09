#pragma once
#include <beet/types.h>
#include <glad/glad.h>
#include <vector>

namespace beet {

constexpr uint32_t MAX_FRAGMENTS = 32;

class ShaderStorageBuffer {
   public:
    ShaderStorageBuffer() = default;
    const void init(const vec2i& size);
    const void resize(const vec2i& size);
    const void unbind() const;

    const void bind_oit(const vec2i& size) const;

   private:
    const void init_ssbo_oit(const vec2i& size);

   private:
    GLuint m_fragmentBuffer{0};
    GLuint m_atomicFragmentIndex{0};
    GLuint m_fragmentStartIndices{0};
    GLuint m_fragmentStartIndicesInit{0};
    struct Fragment {
        uint nextIndex;
        uint packedColor;
        float depth;
        float unusedPadding;
    };
};
}  // namespace beet
