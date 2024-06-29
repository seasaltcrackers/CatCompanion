#pragma once
#include <glew.h>
#include <glm.hpp>

#include <map>
#include <string>

class CProgram;

class CUniformManager
{
public:
	CUniformManager(CProgram* program);
	virtual ~CUniformManager() {}

	void AssignUniforms();
	void UnbindTextures();

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
	void UpdateUniformMat4(std::string uniformName, glm::mat4 mat4);

	void UpdateUniformFloatR(std::string uniformName, float& aFloat);
	void UpdateUniformIntR(std::string uniformName, int& aInt);
	void UpdateUniformfVec2R(std::string uniformName, glm::vec2& vec2);
	void UpdateUniformfVec3R(std::string uniformName, glm::vec3& vec3);
	void UpdateUniformfVec4R(std::string uniformName, glm::vec4& vec4);
	void UpdateUniformiVec2R(std::string uniformName, glm::ivec2& ivec2);
	void UpdateUniformiVec3R(std::string uniformName, glm::ivec3& ivec3);
	void UpdateUniformiVec4R(std::string uniformName, glm::ivec4& ivec4);
	void UpdateUniformMat4R(std::string uniformName, glm::mat4& mat4);

private:
	CProgram* Program;

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
	std::map<GLuint, glm::mat4> UniformsMat4;

	// Holds reference values so you don't have to update the uniform every game frame
	std::map<GLuint, float&> UniformsFloatR;
	std::map<GLuint, int&> UniformsIntR;
	std::map<GLuint, glm::vec2&> UniformsfVec2R;
	std::map<GLuint, glm::vec3&> UniformsfVec3R;
	std::map<GLuint, glm::vec4&> UniformsfVec4R;
	std::map<GLuint, glm::ivec2&> UniformsiVec2R;
	std::map<GLuint, glm::ivec3&> UniformsiVec3R;
	std::map<GLuint, glm::ivec4&> UniformsiVec4R;
	std::map<GLuint, glm::mat4&> UniformsMat4R;
};

