#version 460

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

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

uniform vec4 albedoColor;

vec4 get_color(){
    return albedoColor;//TODO PBR
}

void gather_pass(){
    uint ssboIndex = atomicCounterIncrement(atomicFragmentIndex);
    uint lastIndex = imageAtomicExchange(fragmentStartIndices, ivec2(gl_FragCoord.xy), ssboIndex);

    fragments[ssboIndex].packedColor = packUnorm4x8(get_color());
    fragments[ssboIndex].depth = gl_FragCoord.z;
    fragments[ssboIndex].nextIndex = lastIndex;
}

void main() {
    gather_pass();
}