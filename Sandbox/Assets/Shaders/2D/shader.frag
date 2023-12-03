#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

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
	vec4 Color;
	mat4 SpriteMatrix;
} uniformPush;

void main() {
	outColor = uniformPush.Color;
}
