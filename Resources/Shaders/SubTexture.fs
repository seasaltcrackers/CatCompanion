#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;
uniform ivec4 rect; // x, y, width, height

void main(void)
{
	vec2 texSize = textureSize(tex, 0);
	color = texture(tex, (rect.xy / texSize) + fragTexCoord * (rect.zw / texSize));
}