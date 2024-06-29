#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform vec4 ringColour;
uniform float maxRadius;
uniform float minRadius;

void main(void)
{
	vec2 centerCoord = fragTexCoord - vec2(0.5, 0.5);
	float dist = length(centerCoord) * 2.0f;

	color = vec4(ringColour.xyz, ringColour.w * float(dist <= maxRadius && dist >= minRadius));
}