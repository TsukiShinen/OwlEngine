#version 450

layout(location = 0) in vec2 fragOffset;

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
    vec3 Position;
    vec4 Color;
    float Radius;
} uniformPush;

const float M_PI = 3.1415926538;

void main() {
    float dis = sqrt(dot(fragOffset, fragOffset));
    if (dis >= 1.0) {
        discard;
    }
    outColor = vec4(uniformPush.Color.xyz, cos(dis * M_PI) + 1.0);
}