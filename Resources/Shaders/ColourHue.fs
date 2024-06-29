#version 450 core

#define PI 3.1415926535897932384626433832795
#define PiTimesTwo 6.283185307179586476925286766559

in vec2 fragTexCoord;

out vec4 color;

uniform float maxRadius;
uniform float minRadius;

vec3 hsv2rgb(vec3 c) 
{
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float atan2(float y, float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x, y), atan(y, x), s);
}

void main(void)
{
	vec2 centerCoord = fragTexCoord - vec2(0.5, 0.5);
	float dist = length(centerCoord) * 2.0f;

	float angle = atan2(centerCoord.y, -centerCoord.x);
	angle = mod(angle + PiTimesTwo, PiTimesTwo);
	angle /= PiTimesTwo;

	vec3 hueColour = hsv2rgb(vec3(angle, 1.0f, 1.0f));	
	color = vec4(hueColour, dist <= maxRadius && dist >= minRadius);
}