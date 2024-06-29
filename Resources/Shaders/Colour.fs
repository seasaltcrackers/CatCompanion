#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform vec4 objColour;

void main(void)
{
	color = objColour;
}