#include "uniformManager.h"
#include "program.h"

#include <iostream>
#include <gtc\type_ptr.hpp>

CUniformManager::CUniformManager(CProgram* program)
{
	Program = program;
}

void CUniformManager::AssignUniforms()
{
	// Assign Texture Uniforms
	int i = 0;
	for (std::map<GLuint, GLuint>::iterator it = UniformsTextures.begin(); it != UniformsTextures.end(); it++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, it->second);
		glUniform1i(it->first, i);
		i++;
	}

	// Assign float Uniforms
	for (std::map<GLuint, float>::iterator it = UniformsFloat.begin(); it != UniformsFloat.end(); it++)
	{
		glUniform1f(it->first, it->second);
	}

	// Assign int  Uniforms
	for (std::map<GLuint, int>::iterator it = UniformsInt.begin(); it != UniformsInt.end(); it++)
	{
		glUniform1i(it->first, it->second);
	}

	// Assign fvec2 Uniforms
	for (std::map<GLuint, glm::vec2>::iterator it = UniformsfVec2.begin(); it != UniformsfVec2.end(); it++)
	{
		glUniform2f(it->first, it->second[0], it->second[1]);
	}

	// Assign fvec3 Uniforms
	for (std::map<GLuint, glm::vec3>::iterator it = UniformsfVec3.begin(); it != UniformsfVec3.end(); it++)
	{
		glUniform3f(it->first, it->second[0], it->second[1], it->second[2]);
	}

	// Assign fvec4 Uniforms
	for (std::map<GLuint, glm::vec4>::iterator it = UniformsfVec4.begin(); it != UniformsfVec4.end(); it++)
	{
		glUniform4f(it->first, it->second[0], it->second[1], it->second[2], it->second[3]);
	}

	// Assign ivec2 Uniforms
	for (std::map<GLuint, glm::ivec2>::iterator it = UniformsiVec2.begin(); it != UniformsiVec2.end(); it++)
	{
		glUniform2i(it->first, it->second[0], it->second[1]);
	}

	// Assign ivec3 Uniforms
	for (std::map<GLuint, glm::ivec3>::iterator it = UniformsiVec3.begin(); it != UniformsiVec3.end(); it++)
	{
		glUniform3i(it->first, it->second[0], it->second[1], it->second[2]);
	}

	// Assign ivec4 Uniforms
	for (std::map<GLuint, glm::ivec4>::iterator it = UniformsiVec4.begin(); it != UniformsiVec4.end(); it++)
	{
		glUniform4i(it->first, it->second[0], it->second[1], it->second[2], it->second[3]);
	}

	// Assign mat4 Uniforms
	for (std::map<GLuint, glm::mat4>::iterator it = UniformsMat4.begin(); it != UniformsMat4.end(); it++)
	{
		glUniformMatrix4fv(it->first, 1, GL_FALSE, glm::value_ptr(it->second));
	}

	// Assign float reference Uniforms
	for (std::map<GLuint, float&>::iterator it = UniformsFloatR.begin(); it != UniformsFloatR.end(); it++)
	{
		glUniform1f(it->first, it->second);
	}

	// Assign int reference Uniforms
	for (std::map<GLuint, int&>::iterator it = UniformsIntR.begin(); it != UniformsIntR.end(); it++)
	{
		glUniform1i(it->first, it->second);
	}

	// Assign fvec2 reference Uniforms
	for (std::map<GLuint, glm::vec2&>::iterator it = UniformsfVec2R.begin(); it != UniformsfVec2R.end(); it++)
	{
		glUniform2f(it->first, it->second[0], it->second[1]);
	}

	// Assign fvec3 reference Uniforms
	for (std::map<GLuint, glm::vec3&>::iterator it = UniformsfVec3R.begin(); it != UniformsfVec3R.end(); it++)
	{
		glUniform3f(it->first, it->second[0], it->second[1], it->second[2]);
	}

	// Assign fvec4 reference Uniforms
	for (std::map<GLuint, glm::vec4&>::iterator it = UniformsfVec4R.begin(); it != UniformsfVec4R.end(); it++)
	{
		glUniform4f(it->first, it->second[0], it->second[1], it->second[2], it->second[3]);
	}

	// Assign ivec2 reference Uniforms
	for (std::map<GLuint, glm::ivec2&>::iterator it = UniformsiVec2R.begin(); it != UniformsiVec2R.end(); it++)
	{
		glUniform2i(it->first, it->second[0], it->second[1]);
	}

	// Assign ivec3 reference Uniforms
	for (std::map<GLuint, glm::ivec3&>::iterator it = UniformsiVec3R.begin(); it != UniformsiVec3R.end(); it++)
	{
		glUniform3i(it->first, it->second[0], it->second[1], it->second[2]);
	}

	// Assign ivec4 reference Uniforms
	for (std::map<GLuint, glm::ivec4&>::iterator it = UniformsiVec4R.begin(); it != UniformsiVec4R.end(); it++)
	{
		glUniform4i(it->first, it->second[0], it->second[1], it->second[2], it->second[3]);
	}

	// Assign mat4 reference Uniforms
	for (std::map<GLuint, glm::mat4&>::iterator it = UniformsMat4R.begin(); it != UniformsMat4R.end(); it++)
	{
		glUniformMatrix4fv(it->first, 1, GL_FALSE, glm::value_ptr(it->second));
	}
}

void CUniformManager::UnbindTextures()
{
	for (int i = 0; i < (int)UniformsTextures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

/*
	Update an existing texture uniform or add a new one

	@param uniformName The name of the uniform to be set
	@param textureID The ID of the texture to be assigned
*/
void CUniformManager::UpdateUniformTexture(std::string uniformName, GLuint textureID)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsTextures.insert_or_assign(loc, textureID);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

/*
	Update an existing float uniform or add a new one

	@param uniformName The name of the uniform to be set
	@param aFloat The float to be assigned
*/
void CUniformManager::UpdateUniformFloat(std::string uniformName, float aFloat)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsFloat.insert_or_assign(loc, aFloat);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformInt(std::string uniformName, int aInt)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsInt.insert_or_assign(loc, aInt);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

/*
	Update an existing vec2 uniform or add a new one

	@param uniformName The name of the uniform to be set
	@param vec2 An array of floats (x, y)
*/
void CUniformManager::UpdateUniformfVec2(std::string uniformName, glm::vec2 vec2)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec2.insert_or_assign(loc, vec2);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

/*
	Update an existing vec3 uniform or add a new one

	@param uniformName The name of the uniform to be set
	@param vec3 An array of floats (x, y, z)
*/
void CUniformManager::UpdateUniformfVec3(std::string uniformName, glm::vec3 vec3)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec3.insert_or_assign(loc, vec3);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformfVec4(std::string uniformName, glm::vec4 vec4)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec4.insert_or_assign(loc, vec4);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

/*
	Update an existing ivec2 uniform or add a new one

	@param uniformName The name of the uniform to be set
	@param ivec2 An array of ints (x, y)
*/
void CUniformManager::UpdateUniformiVec2(std::string uniformName, glm::ivec2 ivec2)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsiVec2.insert_or_assign(loc, ivec2);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformiVec3(std::string uniformName, glm::ivec3 ivec3)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsiVec3.insert_or_assign(loc, ivec3);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformiVec4(std::string uniformName, glm::ivec4 ivec4)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsiVec4.insert_or_assign(loc, ivec4);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformMat4(std::string uniformName, glm::mat4 mat4)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsMat4.insert_or_assign(loc, mat4);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformFloatR(std::string uniformName, float& aFloat)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsFloatR.insert_or_assign(loc, aFloat);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformIntR(std::string uniformName, int& aInt)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsIntR.insert_or_assign(loc, aInt);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformfVec2R(std::string uniformName, glm::vec2& vec2)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec2R.insert_or_assign(loc, vec2);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformfVec3R(std::string uniformName, glm::vec3& vec3)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec3R.insert_or_assign(loc, vec3);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformfVec4R(std::string uniformName, glm::vec4& vec4)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec4R.insert_or_assign(loc, vec4);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformiVec2R(std::string uniformName, glm::ivec2& ivec2)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsiVec2R.insert_or_assign(loc, ivec2);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformiVec3R(std::string uniformName, glm::ivec3& ivec3)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsiVec3R.insert_or_assign(loc, ivec3);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformiVec4R(std::string uniformName, glm::ivec4& ivec4)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsiVec4R.insert_or_assign(loc, ivec4);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CUniformManager::UpdateUniformMat4R(std::string uniformName, glm::mat4& mat4)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsMat4R.insert_or_assign(loc, mat4);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}
