#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform int enttHandle;

void main(){
    //read from abgr as handle from entt starts at 0 ie 0x00 00 00 00
    //int a = ((enttHandle >> 24) & 0xFF);
    int b = ((enttHandle >> 16) & 0xFF);
    int g = ((enttHandle >> 8) & 0xFF);
    int r = ((enttHandle >> 0) & 0xFF);

    //    vec4 lightCol = vec4(pointColor_pointIntensity[2].rgb, 1);
    //    FragColor = texture(ourTexture, TexCoords) * lightCol;
    fragColor = vec4(r/255.0, g/255.0, b/255.0, 1.0);

}