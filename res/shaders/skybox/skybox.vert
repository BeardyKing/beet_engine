#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform mat4 invProj;

out vec3 skyboxViewPos;
out vec3 fragPos;

void skybox_view_pos();

void main(){
    skybox_view_pos();

    fragPos = aPos;
    gl_Position = vec4(aPos.xyz, 1.0);
}

void skybox_view_pos(){
    vec3 pos = aPos;
    mat4 invproj = invProj;
    pos.xy *= vec2(invproj[0][0], invproj[1][1]);

    pos.z = -1.0f;
    pos = normalize(pos);

    mat4 transposeView = transpose(view);
    skyboxViewPos = (transposeView * vec4(pos.xyz, 1.0)).xyz;
}