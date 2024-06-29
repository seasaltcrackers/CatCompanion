#pragma once
#include "mesh.h"
#include <glm.hpp>

class CMeshArray :
	public CMesh
{
public:
	CMeshArray(GLuint vao, int elementCount, GLenum drawMode = GL_POINTS);

	void Draw() override;

	static GLuint GenerateVAO(std::vector<GLfloat>& pts);
	static CMeshArray* GeneratePointArray(std::vector<glm::vec3> pts, glm::vec4 colour);
	static CMeshArray* GeneratePointArray(int width, int height, float incr, glm::vec4 colour);
	static CMeshArray* GeneratePatch(float width, float height);

private:
	int ElementCount;
	GLenum DrawMode;
};

