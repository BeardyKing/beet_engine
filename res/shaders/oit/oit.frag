#version 460

struct Fragment {
    uint next_index;
    uint packed_color;
    float depth;
    float unused_padding;
};

layout (early_fragment_tests) in;
layout (binding = 1) uniform atomic_uint atomic_fragment_index;
layout (binding = 6, r32ui) uniform coherent uimage2D fragment_start_indices;
layout (std430, binding = 4) buffer FragmentBlock {
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
    uint ssbo_index = atomicCounterIncrement(atomic_fragment_index);
    uint last_index = imageAtomicExchange(fragment_start_indices, ivec2(gl_FragCoord.xy), ssbo_index);

    fragments[ssbo_index].packed_color = packUnorm4x8(get_color());
    fragments[ssbo_index].depth = gl_FragCoord.z;
    fragments[ssbo_index].next_index = last_index;
}

void main() {
    gather_pass();
}