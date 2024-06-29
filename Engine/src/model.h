#pragma once

#pragma warning(push)
#pragma warning(disable:26495 26812)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#pragma warning(pop)

#include "object.h"
#include "modelMesh.h"


class CModel :
	public CObject
{
public:
	CModel(std::string filepath);
	CModel(std::vector<CModelMesh> modelBase);
	virtual ~CModel() {}

	void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;
	void SetMeshColour(int index, glm::vec3 colour);

	static std::vector<CModelMesh> LoadModelBase(std::string filepath);

private:
	std::vector<CModelMesh> Meshes;
	std::vector<MeshTexture> Textures;
	std::string Directory;

	//void LoadModel(std::string filepath);
	//void ProcessNode(aiNode* node, const aiScene* scene);

	static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<CModelMesh>& meshes, std::string directory);
	static CModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);
	static std::vector<MeshTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<MeshTexture>& textures, std::string directory);
	static GLuint TextureFromFile(const char* path, std::string directory);
};
