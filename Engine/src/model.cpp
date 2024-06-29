#include "model.h"
#include "program.h"

#pragma warning(push)
#pragma warning(disable:26495 26812)
#include <assimp/postprocess.h>
#pragma warning(pop)

#include <iostream>
#include <SOIL.h>

CModel::CModel(std::string filepath) : 
	CObject(CProgram::GetProgram("Model"), nullptr)
{
	Meshes = LoadModelBase(filepath);
}

CModel::CModel(std::vector<CModelMesh> modelBase) :
	CObject(CProgram::GetProgram("Model"), nullptr)
{
	Meshes = modelBase;
}

void CModel::Draw(glm::mat4& pv)
{
	glUseProgram(Program->GetProgramID());	
	AssignUniforms(pv);

	for (GLuint i = 0; i < Meshes.size(); i++)
	{
		Meshes[i].Draw(Program);
	}

	glUseProgram(0);
}

void CModel::SetMeshColour(int index, glm::vec3 colour)
{
	Meshes[index].SetColour(colour);
}

std::vector<CModelMesh> CModel::LoadModelBase(std::string filepath)
{
	std::vector<CModelMesh> meshes;

	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return meshes;
	}

	// Retrieve the directory path of the filepath
	std::string directory = filepath.substr(0, filepath.find_last_of('/'));

	// Process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, meshes, directory);

	return meshes;
}
//
//void CModel::LoadModel(std::string filepath)
//{
//	// Read file via ASSIMP
//	Assimp::Importer importer;
//	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
//
//	// Check for errors
//	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
//	{
//		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
//		return;
//	}
//
//	// Retrieve the directory path of the filepath
//	Directory = filepath.substr(0, filepath.find_last_of('/'));
//
//	// Process ASSIMP's root node recursively
//	ProcessNode(scene->mRootNode, scene);
//}
//
//void CModel::ProcessNode(aiNode* node, const aiScene* scene)
//{
//	// Process each mesh located at the current node
//	for (GLuint i = 0; i < node->mNumMeshes; i++)
//	{
//		// The node object only contains indices to index the actual objects in the scene. 
//		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
//		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//		Meshes.push_back(ProcessMesh(mesh, scene, Directory));
//	}
//
//
//	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
//	for (GLuint i = 0; i < node->mNumChildren; i++)
//	{
//		ProcessNode(node->mChildren[i], scene);
//	}
//}

CModelMesh CModel::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<MeshTexture> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		std::vector<MeshTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures, directory);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 2. Specular maps
		std::vector<MeshTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures, directory);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. Normal Maps
		std::vector<MeshTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", textures, directory);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return CModelMesh(vertices, indices, textures);
}

std::vector<MeshTexture> CModel::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<MeshTexture>& textures, std::string directory)
{
	std::vector<MeshTexture> loadedTextures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < textures.size(); j++)
		{
			// A texture with the same filepath has already been loaded, continue to next one. (optimization)
			if (textures[j].path == str)
			{
				loadedTextures.push_back(textures[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			// If texture hasn't been loaded already, load it
			MeshTexture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str;
			loadedTextures.push_back(texture);
			textures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return loadedTextures;
}

GLuint CModel::TextureFromFile(const char* path, std::string directory)
{
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

void CModel::ProcessNode(aiNode* node, const aiScene* scene, std::vector<CModelMesh>& meshes, std::string directory)
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene, directory));
	}


	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, meshes, directory);
	}
}
