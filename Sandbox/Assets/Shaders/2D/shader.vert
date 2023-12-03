#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

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
	vec4 Color;
	mat4 SpriteMatrix;
} uniformPush;

void main() {
	vec4 positionWorld = uniformPush.SpriteMatrix * vec4(inPosition, 0.0f, 1.0f);
	gl_Position = ubo.Projection * ubo.View * positionWorld;

	fragColor = inColor;
	fragTexCoord = inTexCoord;
}
