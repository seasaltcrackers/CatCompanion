#include "modelMesh.h"
#include "program.h"
#include "camera.h"

#include <sstream>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

CModelMesh::CModelMesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures)
{
	Colour = glm::vec3(0.3f, 0.3f, 0.3f);

	Vertices = vertices;
	Indices = indices;
	Textures = textures;
	// Now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetUpMesh();
}

void CModelMesh::Draw(CProgram* aProgram)
{
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;

	for (GLuint i = 0; i < Textures.size(); i++)
	{
		std::stringstream ss;
		std::string number;
		std::string name = Textures[i].type;

		if (name == "texture_diffuse")
		{
			ss << diffuseNr++; // Transfer GLuint to stream
		}
		else if (name == "texture_specular")
		{
			ss << specularNr++; // Transfer GLuint to stream
		}
		else if (name == "texture_normal")
		{
			ss << normalNr++; // Transfer GLuint to stream
		}

		number = ss.str();

		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(aProgram->GetUniform(name + number), i);
		glBindTexture(GL_TEXTURE_2D, Textures[i].id);
	}

	//glUniform3f(aProgram->GetUniform("baseColour"), Colour.x, Colour.y, Colour.z);

	// Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CModelMesh::SetColour(glm::vec3 colour)
{
	Colour = colour;
}

void CModelMesh::SetUpMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}
