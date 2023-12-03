#version 450

const vec2 OFFSETS[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;

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
    vec3 Position;
    vec4 Color;
    float Radius;
} uniformPush;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];
    vec3 cameraRightWorld = {ubo.View[0][0], ubo.View[1][0], ubo.View[2][0]};
    vec3 cameraUpWorld = {ubo.View[0][1], ubo.View[1][1], ubo.View[2][1]};
    
    vec3 positionWorld = uniformPush.Position.xyz
        + uniformPush.Radius * fragOffset.x * cameraRightWorld
        + uniformPush.Radius * fragOffset.y * cameraUpWorld;
    
    gl_Position = ubo.Projection * ubo.View * vec4(positionWorld, 1.0);
}