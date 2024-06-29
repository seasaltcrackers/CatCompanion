#version 450 core
layout (location = 0) in vec4 vertex; // (vec2:Position, vec2:TexCoords)

out vec2 fragTexCoord;

uniform mat4 proj;

void main()
{
	gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
	fragTexCoord = vertex.zw;
}