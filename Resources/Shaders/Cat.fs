#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;
uniform vec3 lookup[255];

uniform ivec2 splitNumber;
uniform ivec2 spriteIndex;

void main(void)
{
	vec2 spriteSize = (vec2(1, 1) / splitNumber);

	vec4 pixel = texture(tex, fragTexCoord * spriteSize + spriteIndex * spriteSize);
	vec3 lookupColor = abs(lookup[int(round(pixel.x * 255.0f))]);
	color = vec4(lookupColor * pixel.y, pixel.w);
}