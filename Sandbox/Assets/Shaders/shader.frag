#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

struct PointLight {
    vec3 Position;
    vec4 Color;
};

layout(set = 0, binding = 0) uniform GlobalUbo
{
    mat4 Projection;
    mat4 View;
    mat4 InverseView;
    vec4 AmbientLightColor;
    PointLight PointLight[10];
    int NumberLight;
} ubo;

layout(push_constant) uniform Push {
    mat4 ModelMatrix;
    mat4 NormalMatrix;
} uniformPush;

void main() {
    vec3 diffuseLight = ubo.AmbientLightColor.xyz * ubo.AmbientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);
    
    vec3 cameraPositionWorld = ubo.InverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPositionWorld - fragPositionWorld);
    
    for (int i = 0; i < ubo.NumberLight; i++) {
        PointLight light = ubo.PointLight[i];
        vec3 directionToLight = light.Position.xyz - fragPositionWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);
        
        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
        vec3 intensity = light.Color.xyz * light.Color.w * attenuation;
        
        diffuseLight += intensity * cosAngIncidence;
        
        // Specular lighting
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0, 1);
        blinnTerm = pow(blinnTerm, 512.0);
        specularLight += intensity * blinnTerm;
    }
    
    outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}