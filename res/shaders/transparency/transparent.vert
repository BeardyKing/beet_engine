#version 400 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform mat4 model;

void main(){
    vec3 fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fragPos, 1.0);
}