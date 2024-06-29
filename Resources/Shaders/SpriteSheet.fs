#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;
uniform ivec2 splitNumber;
uniform ivec2 spriteIndex;

void main(void)
{
	vec2 spriteSize = (vec2(1, 1) / splitNumber);
	color = texture(tex, fragTexCoord * spriteSize + spriteIndex * spriteSize);
}