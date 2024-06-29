#pragma warning(push)
#pragma warning(disable:26495 26812)
#include <assimp/Importer.hpp>
#pragma warning(pop)

#include <string>
#include <glew.h>
#include <glm.hpp>
#include <vector>


struct Vertex 
{
	glm::vec3 Position; // Position	
	glm::vec3 Normal; // Normal	
	glm::vec2 TexCoords; // TexCoords
};

struct MeshTexture 
{
	GLuint id = 0;
	std::string type;
	aiString path;
};

class CProgram;

class CModelMesh
{
public:
	CModelMesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures);
	void Draw(CProgram* aProgram);

	void SetColour(glm::vec3 colour);

private:
	glm::vec3 Colour;

	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;
	std::vector<MeshTexture> Textures;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	void SetUpMesh();
};