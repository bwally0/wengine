#version 330 core

const float PI = 3.14159265359;
const int MAX_DIRECTIONAL_LIGHTS = 4;
const int MAX_POINT_LIGHTS = 16;
const int MAX_SPOT_LIGHTS = 8;

// Inputs from vertex shader
in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in mat3 v_TBN;

// Output
out vec4 FragColor;

// Material properties
uniform vec3 u_Color;
uniform vec3 u_Emissive;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_Opacity;

// Texture maps
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_EmissiveTexture;

// Texture flags
uniform int u_HasDiffuseTexture;
uniform int u_HasNormalTexture;
uniform int u_HasSpecularTexture;
uniform int u_HasEmissiveTexture;

// Texture transforms
uniform vec2 u_TextureScale;
uniform vec2 u_TextureOffset;

// Camera position
uniform vec3 u_CameraPos;

// Ambient lighting
uniform vec3 u_AmbientColor;

// Light structures
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float innerCutoff;
    float outerCutoff;
    float constant;
    float linear;
    float quadratic;
};

// Light arrays
uniform DirectionalLight u_DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];

// Light counts
uniform int u_NumDirectionalLights;
uniform int u_NumPointLights;
uniform int u_NumSpotLights;

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return a2 / denom;
}

// Geometry Function (Smith's method with Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float denom = NdotV * (1.0 - k) + k;
    
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel-Schlick approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 CalculatePBR(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 H = normalize(V + L);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // Prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic; // Metallic surfaces have no diffuse
    
    float NdotL = max(dot(N, L), 0.0);
    
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(-light.direction);
    vec3 radiance = light.color * light.intensity;
    
    return CalculatePBR(N, V, L, radiance, albedo, metallic, roughness, F0);
}

vec3 CalculatePointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    vec3 radiance = light.color * light.intensity * attenuation;
    
    return CalculatePBR(N, V, L, radiance, albedo, metallic, roughness, F0);
}

vec3 CalculateSpotLight(SpotLight light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Spotlight intensity (soft edges)
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float spotIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    vec3 radiance = light.color * light.intensity * attenuation * spotIntensity;
    
    return CalculatePBR(N, V, L, radiance, albedo, metallic, roughness, F0);
}

void main()
{
    // Apply texture transforms
    vec2 texCoord = v_TexCoord * u_TextureScale + u_TextureOffset;
    
    // Sample albedo (diffuse color)
    vec3 albedo = u_Color;
    if (u_HasDiffuseTexture != 0)
    {
        vec4 texColor = texture(u_DiffuseTexture, texCoord);
        albedo *= texColor.rgb;
    }
    
    // Sample normal map
    vec3 N = v_Normal;
    if (u_HasNormalTexture != 0)
    {
        vec3 tangentNormal = texture(u_NormalTexture, texCoord).xyz * 2.0 - 1.0;
        N = normalize(v_TBN * tangentNormal);
    }
    
    // Sample metallic and roughness
    float metallic = u_Metallic;
    float roughness = u_Roughness;
    if (u_HasSpecularTexture != 0)
    {
        // Assume specular texture stores metallic in R, roughness in G
        vec2 metallicRoughness = texture(u_SpecularTexture, texCoord).rg;
        metallic *= metallicRoughness.r;
        roughness *= metallicRoughness.g;
    }
    
    // Sample emissive
    vec3 emissive = u_Emissive;
    if (u_HasEmissiveTexture != 0)
    {
        emissive += texture(u_EmissiveTexture, texCoord).rgb;
    }
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04); // Base reflectance for dielectrics
    F0 = mix(F0, albedo, metallic); // Metals use albedo as F0
    
    // View direction
    vec3 V = normalize(u_CameraPos - v_FragPos);
    
    // Ambient lighting (simple ambient)
    vec3 ambient = u_AmbientColor * albedo;
    
    // Accumulate lighting from all light sources
    vec3 Lo = vec3(0.0);
    
    // Directional lights
    for (int i = 0; i < u_NumDirectionalLights; i++)
    {
        Lo += CalculateDirectionalLight(u_DirectionalLights[i], N, V, albedo, metallic, roughness, F0);
    }
    
    // Point lights
    for (int i = 0; i < u_NumPointLights; i++)
    {
        Lo += CalculatePointLight(u_PointLights[i], N, V, v_FragPos, albedo, metallic, roughness, F0);
    }
    
    // Spot lights
    for (int i = 0; i < u_NumSpotLights; i++)
    {
        Lo += CalculateSpotLight(u_SpotLights[i], N, V, v_FragPos, albedo, metallic, roughness, F0);
    }
    
    // Final color
    vec3 color = ambient + Lo + emissive;
    
    // Tone mapping (Reinhard)
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, u_Opacity);
}
