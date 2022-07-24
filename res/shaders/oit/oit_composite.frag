#version 460 core

struct Fragment {
    uint next_index;
    uint packed_color;
    float depth;
    float unused_padding;
};

#define MAX_FRAGMENTS 30
#define UINT_MAX 0xFFFFFFFF

Fragment local_fragments[MAX_FRAGMENTS];

layout (std430, binding = 4) buffer FragmentBlock {
    Fragment fragments[];
};

layout (binding = 1, r32ui) uniform coherent uimage2D fragment_start_indices;
layout (location = 0) out vec4 frag_color;
int fragment_count = 0;

void fill_local_fragment_array(){
    uint current_fragment = imageLoad(fragment_start_indices, ivec2(gl_FragCoord.xy)).r;
    while (current_fragment != UINT_MAX && fragment_count < MAX_FRAGMENTS) {
        local_fragments[fragment_count] = fragments[current_fragment];
        current_fragment = local_fragments[fragment_count].next_index;
        fragment_count++;
    }

}

void sort_fragments(){
    for (uint i = 1; i < fragment_count; i++){
        Fragment toInsert = local_fragments[i];
        uint j = i;
        while (j > 0 && toInsert.depth > local_fragments[j-1].depth) {
            local_fragments[j] = local_fragments[j-1];
            j--;
        }
        local_fragments[j] = toInsert;
    }
}

vec4 mix_fragments(){
    float alpha_accumulation = 0;
    vec4 color = vec4(0, 0, 0, 0);
    for (int i = 0; i < fragment_count; i++){
        vec4 col = unpackUnorm4x8(local_fragments[i].packed_color);
        alpha_accumulation = alpha_accumulation + (col.a * col.a);
        color = mix(color, col, col.a);
    }

    return vec4(color.rgb, alpha_accumulation);
}

void main(){
    fill_local_fragment_array();
    sort_fragments();
    frag_color = mix_fragments();
}


