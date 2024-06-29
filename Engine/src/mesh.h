#pragma once
#include <glew.h>
#include <freeglut.h>
#include <vector>
#include <map>
#include <string>


class CMesh
{
public:
	CMesh();
	CMesh(GLuint aVAO, std::vector<GLfloat>& vertices, std::vector<GLuint>& indicies, int stride);
	CMesh(GLuint aVAO, int indiceCount);
	virtual ~CMesh();

	virtual void Draw();

	GLuint GetVAO();
	int GetIndicieCount();
	const std::vector<GLfloat>& GetVertices();
	const std::vector<GLuint>& GetIndicies();
	float GetBoundingRadius();

	// Generate generic meshes
	static GLuint GeneratePTVAO(std::vector<GLfloat>& verts, std::vector<GLuint>& indicies);
	static GLuint GeneratePNTVAO(std::vector<GLfloat>& verts, std::vector<GLuint>& indicies);
	static CMesh* Generate3DRectangle(float width, float height);
	static CMesh* Generate2DRectangle(float width, float height);
	static CMesh* GenerateFBOMesh();
	static CMesh* Generate3DNormaledRectangle(float width, float height);
	static CMesh* GenerateTriangle(float width, float height);
	static CMesh* GeneratePolygon(int sides, float size);
	static CMesh* GeneratePyramid(float size);
	static CMesh* GeneratePyramid2(float size);
	static CMesh* GenerateCube(float size);
	static CMesh* GenerateInvertedCube(float size);
	static CMesh* GenerateNormaledCube(float size);
	static CMesh* GenerateNormaledPyramid(float size);
	static CMesh* GenerateNormaledSphere(float radius, int sectors, int stacks);
	static CMesh* GenerateNormaledCapsule(float radius, float lenght, float sectors, float stacks);

	static void CleanUp();
	static void GenerateMeshes();
	static void InsertMesh(std::string meshName, CMesh* newMesh);
	static CMesh* GetMesh(std::string meshName);

protected:
	GLuint VAO;
	int IndiceCount;
	std::vector<GLfloat> Vertices;
	std::vector<GLuint> Indicies;
	float BoundingRadius;

	static std::map<std::string, CMesh*> Meshes; // Map of all the meshes
};

