#version 460

subroutine void RenderPassType();
subroutine uniform RenderPassType render_pass;

layout (early_fragment_tests) in;

struct Fragment {
    uint NextIndex;
    uint Color;
    float Depth;
    float _pad;

};

layout (binding = 1) uniform atomic_uint atomic_fragment_index;
layout (binding = 6, r32ui) uniform coherent uimage2D fragment_start_indices;
layout (std430, binding = 4) buffer FragmentBlock {
    Fragment fragments[];
};

layout(location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;
uniform vec4 albedoColor;



subroutine(RenderPassType)
void gather_pass(){
    vec4 color = albedoColor;
    //     Get the index of the next empty slot in the buffer
    uint fragment_index = atomicCounterAdd(atomic_fragment_index, 1);
    //
    Fragment fragment;
    fragment.NextIndex = imageAtomicExchange(fragment_start_indices, ivec2(gl_FragCoord.xy), fragment_index);
    fragment.Color = packUnorm4x8(color);
    fragment.Depth = gl_FragCoord.z;
    fragment._pad = 0;// unused
    // save fragment in fragment buffer
    fragments[fragment_index] = fragment;
    FragColor = unpackUnorm4x8(fragments[fragment_index].Color);
}

subroutine(RenderPassType)
void sort_pass(){
    FragColor = vec4(1.0);
}

void main() {
    render_pass();
}
