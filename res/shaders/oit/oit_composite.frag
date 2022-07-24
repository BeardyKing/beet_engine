#version 460 core

struct Fragment {
    uint nextIndex;
    uint packedColor;
    float depth;
    float unusedPadding;
};

#define MAX_FRAGMENTS 30
#define UINT_MAX 0xFFFFFFFF

Fragment localFragments[MAX_FRAGMENTS];

layout (std430, binding = 0) buffer FragmentBlock {
    Fragment fragments[];
};

layout (binding = 1, r32ui) uniform coherent uimage2D fragmentStartIndices;
layout (location = 0) out vec4 fragColor;
int usedFragmentCount = 0;

void fill_local_fragment_array(){
    uint current_fragment = imageLoad(fragmentStartIndices, ivec2(gl_FragCoord.xy)).r;
    while (current_fragment != UINT_MAX && usedFragmentCount < MAX_FRAGMENTS) {
        localFragments[usedFragmentCount] = fragments[current_fragment];
        current_fragment = localFragments[usedFragmentCount].nextIndex;
        usedFragmentCount++;
    }
}

void sort_fragments(){
    for (uint i = 1; i < usedFragmentCount; i++){
        Fragment toInsert = localFragments[i];
        uint j = i;
        while (j > 0 && toInsert.depth > localFragments[j-1].depth) {
            localFragments[j] = localFragments[j-1];
            j--;
        }
        localFragments[j] = toInsert;
    }
}

vec4 mix_fragments(){
    float alphaAccumulation = 0;
    vec4 color = vec4(0, 0, 0, 0);
    for (int i = 0; i < usedFragmentCount; i++){
        vec4 col = unpackUnorm4x8(localFragments[i].packedColor);
        alphaAccumulation = alphaAccumulation + (col.a * col.a);
        color = mix(color, col, col.a);
    }

    return vec4(color.rgb, alphaAccumulation);
}

void main(){
    fill_local_fragment_array();
    sort_fragments();
    fragColor = mix_fragments();
}


