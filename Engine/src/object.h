#pragma once
#include <glew.h>
#include <freeglut.h>
#include <vector>
#include <map>
#include <array>
#include <utility>
#include <string>
#include <functional>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "camera.h"

class CMesh;
class CProgram;

class CObject
{
public:
	CObject();
	CObject(CProgram* aProgram, CMesh* aMesh);
	CObject(CProgram* aProgram, CMesh* aMesh, glm::vec3 aLoc, glm::vec3 aScale = glm::vec3(1.0f, 1.0f, 1.0f), float aRot = 0);
	virtual ~CObject();

	CProgram* GetProgram();
	GLuint GetProgramID();
	CMesh* GetMesh();

	virtual void AssignUniforms(glm::mat4& pv = CCamera::GetProjViewMatrix());
	virtual void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix());
	virtual void Update();

	// Update an existing uniforms or add new one
	void UpdateUniformTexture(std::string uniformName, GLuint textureID);
	void UpdateUniformInt(std::string uniformName, int aInt);
	void UpdateUniformFloat(std::string uniformName, float aFloat);
	void UpdateUniformfVec2(std::string uniformName, glm::vec2 vec2);
	void UpdateUniformfVec3(std::string uniformName, glm::vec3 vec3);
	void UpdateUniformfVec4(std::string uniformName, glm::vec4 vec4);
	void UpdateUniformiVec2(std::string uniformName, glm::ivec2 ivec2);
	void UpdateUniformiVec3(std::string uniformName, glm::ivec3 ivec3);
	void UpdateUniformiVec4(std::string uniformName, glm::ivec4 ivec4);
	void UpdateUniformMat3(std::string uniformName, glm::mat3 mat3);
	void UpdateUniformMat4(std::string uniformName, glm::mat4 mat4);

	void UpdateUniformTextureR(std::string uniformName, GLuint& textureID);
	void UpdateUniformFloatR(std::string uniformName, float& aFloat);
	void UpdateUniformIntR(std::string uniformName, int& aInt);
	void UpdateUniformfVec2R(std::string uniformName, glm::vec2& vec2);
	void UpdateUniformfVec3R(std::string uniformName, glm::vec3& vec3);
	void UpdateUniformfVec4R(std::string uniformName, glm::vec4& vec4);
	void UpdateUniformiVec2R(std::string uniformName, glm::ivec2& ivec2);
	void UpdateUniformiVec3R(std::string uniformName, glm::ivec3& ivec3);
	void UpdateUniformiVec4R(std::string uniformName, glm::ivec4& ivec4);
	void UpdateUniformMat3R(std::string uniformName, glm::mat3& mat3);
	void UpdateUniformMat4R(std::string uniformName, glm::mat4& mat4);

	void UpdateUniformfVec3Array(std::string uniformName, int length, float* pointer);

	// Transforms
	glm::mat4& GetModelMatrix();

	glm::vec3 Location;
	glm::vec3 Scale;

	float RotationZ;
	float RotationX;
	float RotationY;

	glm::vec3 Pivot;

protected:
	CProgram* Program;
	CMesh* Mesh;

	glm::mat4 ModelMatrix; // The current model matrix 
	virtual void UpdateModelMatrix(); // Updates the current model matrix

	void SetAlarm(float seconds, std::function<void()> aFunc); // Sets an alarm function to call in x seconds

private:
	std::vector<std::pair<float, std::function<void()>>> Alarms;

	// Uniform information storage
	std::map<GLuint, GLuint> UniformsTextures;
	std::map<GLuint, float> UniformsFloat;
	std::map<GLuint, int> UniformsInt;
	std::map<GLuint, glm::vec2> UniformsfVec2;
	std::map<GLuint, glm::vec3> UniformsfVec3;
	std::map<GLuint, glm::vec4> UniformsfVec4;
	std::map<GLuint, glm::ivec2> UniformsiVec2;
	std::map<GLuint, glm::ivec3> UniformsiVec3;
	std::map<GLuint, glm::ivec4> UniformsiVec4;
	std::map<GLuint, glm::mat3> UniformsMat3;
	std::map<GLuint, glm::mat4> UniformsMat4;

	// Holds reference values so you don't have to update the uniform every game frame
	std::map<GLuint, GLuint&> UniformsTexturesR;
	std::map<GLuint, float&> UniformsFloatR;
	std::map<GLuint, int&> UniformsIntR;
	std::map<GLuint, glm::vec2&> UniformsfVec2R;
	std::map<GLuint, glm::vec3&> UniformsfVec3R;
	std::map<GLuint, glm::vec4&> UniformsfVec4R;
	std::map<GLuint, glm::ivec2&> UniformsiVec2R;
	std::map<GLuint, glm::ivec3&> UniformsiVec3R;
	std::map<GLuint, glm::ivec4&> UniformsiVec4R;
	std::map<GLuint, glm::mat3&> UniformsMat3R;
	std::map<GLuint, glm::mat4&> UniformsMat4R;

	std::map<GLuint, std::pair<int, float*>> UniformsfVec3Array;
};