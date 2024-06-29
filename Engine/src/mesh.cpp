#include <algorithm>
#include <iostream>

#include "mesh.h"
#include "helper.h"
#include "perlinNoise.h"

std::map<std::string, CMesh*> CMesh::Meshes; // Map of all the meshes

/*
	Initialises an empty mesh
*/
CMesh::CMesh() : VAO(0), IndiceCount(0), BoundingRadius(0.0f) { }

/*
	Initialises a new mesh

	@param aVAO The VAO ID to be passed into the shader
	@param anIndicieCount The number of indices in the VAO
*/
CMesh::CMesh(GLuint aVAO, std::vector<GLfloat>& vertices, std::vector<GLuint>& indicies, int stride)
{
	VAO = aVAO;
	Indicies = indicies;
	//Vertices = vertices;

	IndiceCount = (int)Indicies.size();

	BoundingRadius = 0.0f;

	for (int i = 0; i < (int)vertices.size(); i += stride)
	{
		Vertices.push_back(vertices[i]);
		Vertices.push_back(vertices[i + 1]);
		Vertices.push_back(vertices[i + 2]);

		glm::vec3 point = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
		BoundingRadius = std::max(glm::length(point), BoundingRadius);
	}
}

CMesh::CMesh(GLuint aVAO, int indiceCount)
{
	VAO = aVAO;
	IndiceCount = indiceCount;
	BoundingRadius = -1.0f;
}

/*
	Destroy any dynamic memory
*/
CMesh::~CMesh() { }

/*
	Draw the mesh to the screen
*/
void CMesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, IndiceCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

GLuint CMesh::GetVAO()
{
	return VAO;
}

int CMesh::GetIndicieCount()
{
	return IndiceCount;
}

const std::vector<GLfloat>& CMesh::GetVertices()
{
	return Vertices;
}

const std::vector<GLuint>& CMesh::GetIndicies()
{
	return Indicies;
}

float CMesh::GetBoundingRadius()
{
	return BoundingRadius;
}

/*
	Generates a VAO ID based on a list of vertices and indices

	@param verts A list of vertices
	@param indices A list of Indicies
	@return The GLuint ID of the VAO
*/
GLuint CMesh::GeneratePTVAO(std::vector<GLfloat>& verts, std::vector<GLuint>& indices)
{
	GLuint vao = 0;
	GLuint ebo = 0;
	GLuint vbo = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,								// Maps to the layout location
		3,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		5 * sizeof(GLfloat),			// Step Size
		(GLvoid*)0						// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,								// Maps to the layout location
		2,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		5 * sizeof(GLfloat),			// Step Size
		(GLvoid*)(3 * sizeof(GLfloat))	// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	return vao;
}

GLuint CMesh::GeneratePNTVAO(std::vector<GLfloat>& verts, std::vector<GLuint>& indices)
{
	GLuint vao = 0;
	GLuint ebo = 0;
	GLuint vbo = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,								// Maps to the layout location
		3,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		8 * sizeof(GLfloat),			// Step Size
		(GLvoid*)0						// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,								// Maps to the layout location
		3,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		8 * sizeof(GLfloat),			// Step Size
		(GLvoid*)(3 * sizeof(GLfloat))	// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		2,								// Maps to the layout location
		2,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		8 * sizeof(GLfloat),			// Step Size
		(GLvoid*)(6 * sizeof(GLfloat))	// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return vao;
}

/*
	Generates a rectangle middle centered, based off the 3D coordinate system (will appear upsidedown using a 2D camera)

	@param width The width of the rectangle
	@param height The height of the rectangle
	@return The rectangles mesh
*/
CMesh* CMesh::Generate3DRectangle(float width, float height)
{
	width *= 0.5;
	height *= 0.5;

	std::vector<GLfloat> tempVerts = {
		// Position					 // Textured Coords
		-width,  height, 0.0f,		 0.0f,   0.0f,  // Top - Left
		-width, -height, 0.0f,		 0.0f,   1.0f,  // Bot - Left
		 width, -height, 0.0f,		 1.0f,   1.0f,  // Bot - Right
		 width,  height, 0.0f,		 1.0f,   0.0f,  // Top - Right
	};

	std::vector<GLuint> tempIndices = {
		0, 1, 2, // First Triangle
		0, 2, 3, // Second Triangle
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}


CMesh* CMesh::Generate3DNormaledRectangle(float width, float height)
{
	width *= 0.5;
	height *= 0.5;

	std::vector<GLfloat> tempVerts = {
		// Position					// Normal					// Textured Coords
		-width,  height, 0.0f,		0.0f,	0.0f,	1.0f,		0.0f,	0.0f,  // Top - Left
		-width, -height, 0.0f,		0.0f,	0.0f,	1.0f,		0.0f,	1.0f,  // Bot - Left
		 width, -height, 0.0f,		0.0f,	0.0f,	1.0f,		1.0f,	1.0f,  // Bot - Right
		 width,  height, 0.0f,		0.0f,	0.0f,	1.0f,		1.0f,	0.0f,  // Top - Right
	};

	std::vector<GLuint> tempIndices = {
		0, 1, 2, // First Triangle
		0, 2, 3, // Second Triangle
	};

	GLuint vao = GeneratePNTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 8);
}

CMesh* CMesh::GenerateTriangle(float width, float height)
{
	std::vector<GLfloat> tempVerts = {
		// Position									// Textured Coords
		0.0f,			height * 0.5f,	0.0f,				0.0f,	1.0f,  // Bot - Left
		width * 0.5f,	height * -0.5f,	0.0f,				1.0f,	1.0f,  // Bot - Right
		width * -0.5f,	height * -0.5f,	0.0f,				1.0f,	0.0f,  // Top - Right
	};

	std::vector<GLuint> tempIndices = {
		0, 1, 2,
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

/*
	Generates a rectangle centered top left based off the 2D coordinate system (will appear upsidedown using a 3D camera)

	@param width The width of the rectangle
	@param height The height of the rectangle
	@return The rectangles mesh
*/
CMesh* CMesh::Generate2DRectangle(float width, float height)
{
	std::vector<GLfloat> tempVerts = {
		// Position					// Textured Coords
		0.0f,	0.0f,	0.0f,		 0.0f,	0.0f,  // Top - Left
		0.0f,	height,	0.0f,		 0.0f,	1.0f,  // Bot - Left
		width,	height,	0.0f,		 1.0f,	1.0f,  // Bot - Right
		width,	0.0f,	0.0f,		 1.0f,	0.0f,  // Top - Right
	};

	std::vector<GLuint> tempIndices = {
		0, 1, 2, // First Triangle
		0, 2, 3, // Second Triangle
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

CMesh* CMesh::GenerateFBOMesh()
{
	std::vector<GLfloat> tempVerts = {
		// Position					 // Textured Coords
		-1.0f, -1.0f, 0.0f,		 0.0f,   0.0f,  // Top - Left
		-1.0f,  1.0f, 0.0f,		 0.0f,   1.0f,  // Bot - Left
		 1.0f,  1.0f, 0.0f,		 1.0f,   1.0f,  // Bot - Right
		 1.0f, -1.0f, 0.0f,		 1.0f,   0.0f,  // Top - Right
	};

	std::vector<GLuint> tempIndices = {
		0, 2, 1, // First Triangle
		0, 3, 2, // Second Triangle
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}


/*
	Generates a centered polygon with x amount of sides spread evenly

	@param sides The number of sides
	@param size The radius of the polygon (from the center to each point)
	@return The polygon mesh
*/
CMesh* CMesh::GeneratePolygon(int sides, float size)
{
	std::vector<GLfloat> tempVerts;
	std::vector<GLuint> tempIndices;

	// Outter Vertices
	float increment = (float)(2.0f * PI) / (float)sides;
	for (int i = 0; i < sides; ++i)
	{
		float x = HELPER::LengthDirRadX(size, increment * i);
		float y = HELPER::LengthDirRadY(size, increment * i);

		// Vertex Data:
		tempVerts.push_back(x);		// x
		tempVerts.push_back(y);		// y
		tempVerts.push_back(0.0f);	// z

		tempVerts.push_back(0.5f + x / size / 2.0f);	// UV x
		tempVerts.push_back(0.5f - y / size / 2.0f);	// UV y

		// Indicies:
		tempIndices.push_back(i);
		tempIndices.push_back((i + 1) % sides);
		tempIndices.push_back(sides);
	}

	// Middle Vertex
	tempVerts.push_back(0.0f);	// x
	tempVerts.push_back(0.0f);	// y
	tempVerts.push_back(0.0f);	// z

	tempVerts.push_back(0.5f);	// UV x
	tempVerts.push_back(0.5f);	// UV y

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

CMesh* CMesh::GeneratePyramid(float size)
{
	std::vector<GLfloat> tempVerts = {
		// Positions			// Tex Coords
		-size, 0.0f, -size,		0.0f, 0.0f, // 0
		-size, 0.0f,  size,		0.0f, 1.0f, // 1
		 size, 0.0f,  size,		1.0f, 1.0f, // 2
		 size, 0.0f, -size,		1.0f, 0.0f, // 3

		 0.0f, size, 0.0f,		0.5f, 0.5f, // Top point
	};

	std::vector<GLuint> tempIndices = {
		0, 4, 3,
		3, 4, 2,
		2, 4, 1,
		1, 4, 0,

		1, 0, 3,
		1, 3, 2,
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

CMesh* CMesh::GeneratePyramid2(float size)
{
	size *= 0.5f;

	std::vector<GLfloat> tempVerts = {
		-size, 0.0f, -size,		0.0f, 1.0f,
		-size, 0.0f,  size,		0.0f, 0.0f,
		 size, 0.0f,  size,		1.0f, 0.0f,
		 size, 0.0f, -size,		1.0f, 1.0f,

		-size, 0.0f, -size,		1.0f, 1.0f,
		 size, 0.0f, -size,		0.0f, 1.0f,

		 size, 0.0f, -size,		1.0f, 1.0f,
		 size, 0.0f,  size,		0.0f, 1.0f,

		 size, 0.0f,  size,		1.0f, 1.0f,
		-size, 0.0f,  size,		0.0f, 1.0f,

		-size, 0.0f,  size,		1.0f, 1.0f,
		-size, 0.0f, -size,		0.0f, 1.0f,

		 0.0f, size * 2.0f, 0.0f,		0.5f, 0.0f,
	};

	std::vector<GLuint> tempIndices = {
		1,  0,  3,
		1,  3,  2,

		4,  12, 5,
		6,  12, 7,
		8,  12, 9,
		10, 12, 11,
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

CMesh* CMesh::GenerateCube(float size)
{
	size *= 0.5f;

	std::vector<GLfloat> tempVerts = {
		// Front
		-size,  size,  size,		0.0f, 0.0f,
		-size, -size,  size,		0.0f, 1.0f,
		 size, -size,  size,		1.0f, 1.0f,
		 size,  size,  size,		1.0f, 0.0f,

		 // Back
		-size,  size, -size,		1.0f, 0.0f,
		-size, -size, -size,		1.0f, 1.0f,
		 size, -size, -size,		0.0f, 1.0f,
		 size,  size, -size,		0.0f, 0.0f,

		 // Left
		-size,  size, -size,		0.0f, 0.0f,
		-size, -size, -size,		0.0f, 1.0f,
		-size, -size,  size,		1.0f, 1.0f,
		-size,  size,  size,		1.0f, 0.0f,

		// Right
		 size,  size,  size,		0.0f, 0.0f,
		 size, -size,  size,		0.0f, 1.0f,
		 size, -size, -size,		1.0f, 1.0f,
		 size,  size, -size,		1.0f, 0.0f,

		 // Top
		 -size,  size, -size,		0.0f, 0.0f,
		 -size,  size,  size,		0.0f, 1.0f,
		  size,  size,  size,		1.0f, 1.0f,
		  size,  size, -size,		1.0f, 0.0f,

		  // Bottom
		  -size, -size,  size,		0.0f, 0.0f,
		  -size, -size, -size,		0.0f, 1.0f,
		   size, -size, -size,		1.0f, 1.0f,
		   size, -size,  size,		1.0f, 0.0f,

	};

	std::vector<GLuint> tempIndices = {
		0,	1,	2,
		0,	2,	3,

		7,	6,	5,
		7,	5,	4,

		8,	9,	10,
		8,	10,	11,

		12,	13,	14,
		12,	14,	15,

		16,	17,	18,
		16,	18,	19,

		20,	21,	22,
		20,	22,	23
	};

	GLuint vao = GeneratePTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

CMesh* CMesh::GenerateInvertedCube(float size)
{
	size *= 0.5f;

	std::vector<GLfloat> tempVerts = {
		// Front
		-size,  size,  size,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		-size, -size,  size,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		 size, -size,  size,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		 size,  size,  size,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,

		 // Back
		-size,  size, -size,	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		-size, -size, -size,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		 size, -size, -size,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		 size,  size, -size,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

		 // Left
		-size,  size, -size,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		-size, -size, -size,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-size, -size,  size,	-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-size,  size,  size,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

		// Right
		 size,  size,  size,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		 size, -size,  size,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		 size, -size, -size,	1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		 size,  size, -size,	1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

		 // Top
		 -size,  size, -size,	0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 -size,  size,  size,	0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		  size,  size,  size,	0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		  size,  size, -size,	0.0f, 1.0f, 0.0f,		1.0f, 0.0f,

		  // Bottom
		  -size, -size,  size,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		  -size, -size, -size,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		   size, -size, -size,	0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		   size, -size,  size,	0.0f, -1.0f, 0.0f,		1.0f, 0.0f,

	};

	std::vector<GLuint> tempIndices = {
		2, 1,  0,
		3, 2,  0,

		5, 6,  7,
		4, 5,  7,

		10,9,  8,
		11,10, 8,

		14, 13,	12,
		15, 14,	12,

		18, 17,	16,
		19, 18,	16,

		22, 21,	20,
		23, 22, 20,
	};

	GLuint vao = GeneratePNTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 8);
}

CMesh* CMesh::GenerateNormaledCube(float size)
{
	size *= 0.5f;

	std::vector<GLfloat> tempVerts = {
		// Front
		-size,  size,  size,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		-size, -size,  size,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		 size, -size,  size,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		 size,  size,  size,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,

		 // Back
		-size,  size, -size,	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		-size, -size, -size,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		 size, -size, -size,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		 size,  size, -size,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

		 // Left
		-size,  size, -size,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		-size, -size, -size,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-size, -size,  size,	-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-size,  size,  size,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

		// Right
		 size,  size,  size,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		 size, -size,  size,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		 size, -size, -size,	1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		 size,  size, -size,	1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

		 // Top
		 -size,  size, -size,	0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 -size,  size,  size,	0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		  size,  size,  size,	0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		  size,  size, -size,	0.0f, 1.0f, 0.0f,		1.0f, 0.0f,

		  // Bottom
		  -size, -size,  size,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		  -size, -size, -size,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		   size, -size, -size,	0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		   size, -size,  size,	0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	};

	std::vector<GLuint> tempIndices = {
		0,	1,	2,
		0,	2,	3,

		7,	6,	5,
		7,	5,	4,

		8,	9,	10,
		8,	10,	11,

		12,	13,	14,
		12,	14,	15,

		16,	17,	18,
		16,	18,	19,

		20,	21,	22,
		20,	22,	23
	};

	GLuint vao = GeneratePNTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 8);
}

CMesh* CMesh::GenerateNormaledPyramid(float size)
{
	std::vector<GLfloat> tempVerts = {
		// Positions			// Normals					// Tex Coords
		-size, 0.0f, -size,		0.0f, -1.0f, 0.0f,			0.0f, 0.0f, // 0
		-size, 0.0f,  size,		0.0f, -1.0f, 0.0f,			0.0f, 1.0f, // 1
		 size, 0.0f,  size,		0.0f, -1.0f, 0.0f,			1.0f, 1.0f, // 2
		 size, 0.0f, -size,		0.0f, -1.0f, 0.0f,			1.0f, 0.0f, // 3

		 0.0f, size, 0.0f,		0.0f, 1.0f, 0.0f,			0.5f, 0.5f, // Top point
	};

	std::vector<GLuint> tempIndices = {
		0, 4, 3,
		3, 4, 2,
		2, 4, 1,
		1, 4, 0,

		1, 0, 3,
		1, 3, 2,
	};

	GLuint vao = GeneratePNTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 5);
}

CMesh* CMesh::GenerateNormaledSphere(float radius, int sectors, int stacks)
{
	std::vector<GLfloat> tempVerts;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2.0f * (float)PI / (float)sectors;
	float stackStep = (float)PI / (float)stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = (float)PI / 2.0f - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			tempVerts.push_back(x);
			tempVerts.push_back(y);
			tempVerts.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			tempVerts.push_back(nx);
			tempVerts.push_back(ny);
			tempVerts.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectors;
			t = (float)i / stacks;
			tempVerts.push_back(s);
			tempVerts.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles
	std::vector<GLuint> tempIndices;

	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				tempIndices.push_back(k1);
				tempIndices.push_back(k2);
				tempIndices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				tempIndices.push_back(k1 + 1);
				tempIndices.push_back(k2);
				tempIndices.push_back(k2 + 1);
			}
		}
	}

	GLuint vao = GeneratePNTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 8);
}

CMesh* CMesh::GenerateNormaledCapsule(float radius, float length, float sectors, float stacks)
{
	std::vector<GLfloat> tempVerts;

	float x, z, y, xz;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2.0f * (float)PI / (float)sectors;
	float stackStep = (float)PI / (float)stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = (float)PI / 2.0f - i * stackStep;		// starting from pi/2 to -pi/2
		xz = radius * cosf(stackAngle);						// r * cos(u)
		y = radius * sinf(stackAngle);						// r * sin(u)


		float multiplication = i > stacks * 0.5f ? -1.0f : 1.0f;

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j)
		{
			//float multiplication = j > sectors * 0.5f ? -1.0f : 1.0f;

			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
			z = xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
			tempVerts.push_back(x);
			tempVerts.push_back(y + length * 0.5f * multiplication);
			tempVerts.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			tempVerts.push_back(nx);
			tempVerts.push_back(ny);
			tempVerts.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectors;
			t = (float)i / stacks;
			tempVerts.push_back(s);
			tempVerts.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles
	std::vector<GLuint> tempIndices;

	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = int(i * (sectors + 1));     // beginning of current stack
		k2 = int(k1 + sectors + 1);      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				tempIndices.push_back(k1);
				tempIndices.push_back(k2);
				tempIndices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				tempIndices.push_back(k1 + 1);
				tempIndices.push_back(k2);
				tempIndices.push_back(k2 + 1);
			}
		}
	}

	GLuint vao = GeneratePNTVAO(tempVerts, tempIndices);
	return new CMesh(vao, tempVerts, tempIndices, 8);
}

/*
	Cleans up the dynamic memory created by Meshes map
*/
void CMesh::CleanUp()
{
	std::for_each(Meshes.begin(), Meshes.end(), [](std::pair<const std::string, CMesh*>& it) { delete it.second; });
	Meshes.clear();
}

void CMesh::GenerateMeshes()
{
	CMesh::InsertMesh("FBO Mesh", CMesh::GenerateFBOMesh());
	CMesh::InsertMesh("3D Square", CMesh::Generate3DRectangle(1.0f, 1.0f));
	CMesh::InsertMesh("2D Square", CMesh::Generate2DRectangle(1.0f, 1.0f));
	CMesh::InsertMesh("3D NSquare", CMesh::Generate3DNormaledRectangle(1.0f, 1.0f));
	CMesh::InsertMesh("Triangle", CMesh::GenerateTriangle(1.0f, 1.0f));
	CMesh::InsertMesh("Pentagon", CMesh::GeneratePolygon(5, 0.5f));
	CMesh::InsertMesh("Hexagon", CMesh::GeneratePolygon(6, 0.5f));
	CMesh::InsertMesh("Circle", CMesh::GeneratePolygon(50, 0.5f));
	CMesh::InsertMesh("Pyramid", CMesh::GeneratePyramid(1.0f));
	CMesh::InsertMesh("Pyramid2", CMesh::GeneratePyramid2(1.0f));
	CMesh::InsertMesh("Cube", CMesh::GenerateCube(1.0f));
	CMesh::InsertMesh("ICube", CMesh::GenerateInvertedCube(1.0f));
	CMesh::InsertMesh("NCube", CMesh::GenerateNormaledCube(1.0f));
	CMesh::InsertMesh("NSphere", CMesh::GenerateNormaledSphere(1.0f, 100, 100));
	CMesh::InsertMesh("NPyramid", CMesh::GeneratePyramid(1.0f));
	CMesh::InsertMesh("NCapsule", CMesh::GenerateNormaledCapsule(1.0f, 2.0f, 100, 100));
}

void CMesh::InsertMesh(std::string meshName, CMesh* newMesh)
{
	std::cout << "Generated mesh '" + meshName + "'" << std::endl;
	CMesh::Meshes.insert({ meshName, newMesh });
}

CMesh* CMesh::GetMesh(std::string meshName)
{
	auto it = Meshes.find(meshName);
	if (it != Meshes.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "WARNING: Failed to find MESH '" << meshName << "'" << std::endl;
		return nullptr;
	}
}
