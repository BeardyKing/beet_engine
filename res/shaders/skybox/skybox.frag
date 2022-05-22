#version 330 core

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform sampler2D albedo;
uniform sampler2D irradiance;
uniform sampler2D radiance;

uniform vec4 skyboxData;

#define exposure skyboxData.x
#define skyboxType skyboxData.y

#define PI 3.14159265

in vec3 fragPos;
in vec3 skyboxViewPos;
out vec4 fragColor;

vec2 equirectangular_to_view_uv(vec3 view);
vec3 linear(vec3 color);
vec4 linear(vec4 color);
vec3 filmic(vec3 color);
vec4 filmic(vec4 color);

void main(){
    vec4 color;
    float lod = 1.0f;

    vec2 uv = equirectangular_to_view_uv(skyboxViewPos);

    if (skyboxType == 0.0) {
        color = textureLod(albedo, uv, lod);
    } else if (skyboxType == 1.0) {
        color = textureLod(irradiance, uv, lod);
    } else if (skyboxType == 2.0) {
        color = textureLod(radiance, uv, lod);
    }

    color = linear(color);
    color *= exp2(exposure);
    color = filmic(color);

    fragColor = color;
}

vec2 equirectangular_to_view_uv(vec3 view) {
    view = normalize(view);
    return vec2(atan(view.z, view.x) + PI, acos(view.y)) / vec2(2.0 * PI, PI);
}

//https://github.com/bkaradzic/bgfx/blob/master/examples/common/shaderlib.sh
vec3 linear(vec3 color){
    return pow(abs(color), vec3(2.2));
}

vec4 filmic(vec4 color){
    return vec4(filmic(color.rgb), color.a);
}

vec4 linear(vec4 color){
    return vec4(linear(color.rgb), color.a);
}

vec3 filmic(vec3 color){
    color = max(vec3(0.0), color - 0.004);
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}


