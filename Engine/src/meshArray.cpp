#include "meshArray.h"

CMeshArray::CMeshArray(GLuint vao, int elementCount, GLenum drawMode) :
	CMesh(vao, -1)
{
	ElementCount = elementCount;
	DrawMode = drawMode;
}

void CMeshArray::Draw()
{
	glBindVertexArray(GetVAO());
	glDrawArrays(DrawMode, 0, ElementCount);
	glBindVertexArray(0);
}

GLuint CMeshArray::GenerateVAO(std::vector<GLfloat>& pts)
{
	GLuint vao = 0;
	GLuint vbo = 0;

	// Generate VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(GLfloat), pts.data(), GL_STATIC_DRAW);

	// Match to the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);

	// Match to the colour
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	// Unbind the vao
	glBindVertexArray(0);
	return vao;
}

CMeshArray* CMeshArray::GeneratePointArray(std::vector<glm::vec3> pts, glm::vec4 colour)
{
	std::vector<GLfloat> points;

	for (glm::vec3& vertex : pts)
	{
		points.push_back(vertex.x);
		points.push_back(vertex.y);
		points.push_back(vertex.z);

		points.push_back(colour.x);
		points.push_back(colour.y);
		points.push_back(colour.z);
		points.push_back(colour.w);
	}

	return new CMeshArray(GenerateVAO(points), pts.size());
}

CMeshArray* CMeshArray::GeneratePointArray(int width, int height, float incr, glm::vec4 colour)
{
	float xOff = (float)width * 0.5f * incr;
	float zOff = (float)height * 0.5f * incr;

	std::vector<GLfloat> points;

	for (int x = 0; x < width; ++x)
	{
		for (int z = 0; z < height; ++z)
		{
			points.push_back(x * incr - xOff);
			points.push_back(0.0f);
			points.push_back(z * incr - zOff);

			points.push_back(colour.x);
			points.push_back(colour.y);
			points.push_back(colour.z);
			points.push_back(colour.w);
		}
	}

	return new CMeshArray(GenerateVAO(points), width * height);
}

CMeshArray* CMeshArray::GeneratePatch(float width, float height)
{
	float w = width * 0.5f;
	float h = height * 0.5f;

	std::vector<GLfloat> points = {
		-w, 0.0f, -h,
		 w, 0.0f, -h,
		 w, 0.0f,  h,
		-w, 0.0f,  h
	};


	GLuint vao = 0;
	GLuint vbo = 0;

	// Generate VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

	// Match to the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	// Unbind the vao
	glBindVertexArray(0);

	return new CMeshArray(vao, 4, GL_PATCHES);
}
