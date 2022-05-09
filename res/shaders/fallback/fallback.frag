#version 330 core

const int MAX_POINT_LIGHTS = 128;
layout (std140) uniform PointLights{
    vec4 pointPosition_pointRange[MAX_POINT_LIGHTS];
    vec4 pointColor_pointIntensity[MAX_POINT_LIGHTS];
};

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

out vec4 FragColor;
in vec4 color;
in vec2 TexCoords;

uniform sampler2D ourTexture;

void main(){
    //    vec4 lightCol = vec4(pointColor_pointIntensity[2].rgb, 1);
    vec4 lightCol = vec4(1);
    FragColor = texture(ourTexture, TexCoords) * lightCol;
}