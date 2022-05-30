#version 420 core
layout (location = 0) out vec4 fragColor;
layout (location = 1) out float reveal;


in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

const int MAX_POINT_LIGHTS = 128;
layout (std140) uniform PointLights{
    vec4 amountOfActiveLights_unused_unused_unused;
    vec4 pointPosition_pointRange[MAX_POINT_LIGHTS];
    vec4 pointColor_pointIntensity[MAX_POINT_LIGHTS];
};
float amountOfActiveLights = amountOfActiveLights_unused_unused_unused.x;


layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D occlusionMap;

uniform vec4 albedoColor;
uniform vec2 textureTiling;
uniform float albedoScalar;
uniform float normalScalar;
uniform float metallicScalar;
uniform float roughnessScalar;
uniform float occlusionScalar;
uniform float skyboxScalar;
uniform int castShadows;
uniform int receivesShadows;
uniform int alphaCutoffEnabled;
uniform float alphaCutoffAmount;
uniform int enttHandle;
uniform bool isOpaque;

const float PI = 3.14159265359;
const float EPSILON = 0.0001;

vec3 getNormalFromMap(){
    vec3 tangentNormal = texture(normalMap, fs_in.texCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(viewPos);
    vec3 Q2  = dFdy(viewPos);
    vec2 st1 = dFdx(fs_in.texCoords);
    vec2 st2 = dFdy(fs_in.texCoords);

    vec3 N   = normalize(fs_in.normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main(){

    vec3 ambientLightCol = vec3(1.0, 0.9, 1.0);

    vec3 objectColor = albedoColor.rgb;

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * ambientLightCol;
    vec3 diffuse = vec3(0);
    // diffuse
    for (int i = 0; i < amountOfActiveLights; ++i){
        vec3 lightPos = pointPosition_pointRange[i].xyz;
        vec3 norm = normalize(fs_in.normal);
        vec3 lightDir = normalize(lightPos - fs_in.fragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += diff * pointColor_pointIntensity[i].xyz;
    }
    vec3 result = (ambient + diffuse) * objectColor;

    //read from abgr as handle from entt starts at 0 ie 0x00 00 00 00
    //int a = ((enttHandle >> 24) & 0xFF);
    int b = ((enttHandle >> 16) & 0xFF);
    int g = ((enttHandle >> 8) & 0xFF);
    int r = ((enttHandle >> 0) & 0xFF);

    //    vec4 lightCol = vec4(pointColor_pointIntensity[2].rgb, 1);
    //    FragColor = texture(ourTexture, TexCoords) * lightCol;
    //    fragColor = vec4(r/255.0, g/255.0, b/255.0, 1.0);

    if (isOpaque){
        fragColor = vec4(result, 1.0);
    }
    else {
        float alphaAmount = albedoColor.a;
        float weight = clamp(pow(min(1.0, albedoColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
        //        vec4(result, 1.0)
        // store pixel color accumulation
        fragColor = vec4(result.rgb * albedoColor.a, albedoColor.a) * weight;

        // store pixel revealage threshold
        reveal = alphaAmount;

        //    reveal = albedoColor.a;
        vec2 uv = fs_in.texCoords;
        reveal = texture(albedoMap, uv).a * albedoColor.a;
    }

    //
    //    vec3 albedo     = pow(texture(albedoMap, uv).rgb, vec3(2.2));
    //    float metallic  = texture(metallicMap, uv).r;
    //    float roughness = texture(roughnessMap, uv).r;
    //    float ao        = texture(occlusionMap, uv).r;
    //
    //    vec3 N = getNormalFromMap();
    //    vec3 V = normalize(viewPos - fs_in.fragPos);
    //
    //    vec3 F0 = vec3(0.04);
    //    F0 = mix(F0, albedo, metallic);
    //
    //    // reflectance equation
    //    vec3 Lo = vec3(0.0);
    //    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    //    {
    //        vec3 lightPositions = pointPosition_pointRange[i].xyz;
    //        vec3 lightColors = pointColor_pointIntensity[i].xyz;
    //        // calculate per-light radiance
    //        vec3 L = normalize(lightPositions[i] - fs_in.fragPos);
    //        vec3 H = normalize(V + L);
    //        float distance = length(lightPositions[i] - fs_in.fragPos);
    //        float attenuation = 1.0 / (distance * distance);
    //        vec3 radiance = lightColors * attenuation;
    //
    //        // Cook-Torrance BRDF
    //        float NDF = DistributionGGX(N, H, roughness);
    //        float G   = GeometrySmith(N, V, L, roughness);
    //        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
    //
    //        vec3 nominator    = NDF * G * F;
    //        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.01;// 0.001 to prevent divide by zero.
    //        vec3 specular = nominator / denominator;
    //
    //        // kS is equal to Fresnel
    //        vec3 kS = F;
    //        // for energy conservation, the diffuse and specular light can't
    //        // be above 1.0 (unless the surface emits light); to preserve this
    //        // relationship the diffuse component (kD) should equal 1.0 - kS.
    //        vec3 kD = vec3(1.0) - kS;
    //        // multiply kD by the inverse metalness such that only non-metals
    //        // have diffuse lighting, or a linear blend if partly metal (pure metals
    //        // have no diffuse light).
    //        kD *= 1.0 - metallic;
    //
    //        // scale light by NdotL
    //        float NdotL = max(dot(N, L), 0.0);
    //
    //        // add to outgoing radiance Lo
    //        //        float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //        Lo += (kD * albedo / PI + specular) * ((radiance) * NdotL);// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    //    }
    //
    //    vec3 ambient = vec3(0.03) * albedo * ao;
    //
    //    vec3 color = ambient + (Lo);
    //
    //    color = color / (color + vec3(1.0));
    //    color = pow(color, vec3(1.0/2.2));
    //
    //    fragColor = vec4(color, 1.0);
}