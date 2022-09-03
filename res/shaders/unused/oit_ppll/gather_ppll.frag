#version 460

//Per-Pixel Linked List

struct Fragment {
    uint nextIndex;
    uint packedColor;
    float depth;
    float unusedPadding;
};

layout (early_fragment_tests) in;
layout (binding = 1) uniform atomic_uint atomicFragmentIndex;
layout (binding = 6, r32ui) uniform coherent uimage2D fragmentStartIndices;
layout (std430, binding = 0) buffer FragmentBlock {
    Fragment fragments[];
};

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform vec4 albedoColor;

void gather_pass(){
    vec4 outColor = albedoColor;

    uint ssboIndex = atomicCounterIncrement(atomicFragmentIndex);
    uint lastIndex = imageAtomicExchange(fragmentStartIndices, ivec2(gl_FragCoord.xy), ssboIndex);

    fragments[ssboIndex].packedColor = packUnorm4x8(outColor);
    fragments[ssboIndex].depth = gl_FragCoord.z;
    fragments[ssboIndex].nextIndex = lastIndex;
}

void main() {
    gather_pass();
}