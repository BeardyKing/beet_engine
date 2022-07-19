#version 400 core

// shader outputs
layout (location = 0) out vec4 fragColor;

// material color
uniform vec4 albedoColor;

void main()
{
    fragColor = vec4(albedoColor.rgb, 1.0);
}

