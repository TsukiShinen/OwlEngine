#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inUv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec3 fragNormalWorld;

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
    vec4 positionWorld = uniformPush.ModelMatrix * vec4(inPosition, 1.0f);
    gl_Position = ubo.Projection * ubo.View * positionWorld;

    fragNormalWorld = normalize(mat3(uniformPush.NormalMatrix) * inNormal);
    fragPositionWorld = positionWorld.xyz;
    fragColor = inColor;
}
