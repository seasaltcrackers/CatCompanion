#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;
uniform vec4 textColor;

void main()
{
	color = vec4(textColor.xyz, texture(tex, fragTexCoord).r * textColor.w);
}