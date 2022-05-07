#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
};

out vec2 TexCoord;
out vec4 color;

uniform mat4 model;
uniform vec4 albedoColor;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    color = albedoColor;
}