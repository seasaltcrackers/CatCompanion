#include <iostream>

#include "object.h"
#include "helper.h"
#include "program.h"
#include "mesh.h"
#include "camera.h"
#include "timer.h"
#include "graphics.h"

/*
	Initialises an empty new object
*/
CObject::CObject() : Program(0), Mesh(0)
{
	Pivot = glm::vec3();
	Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	Location = glm::vec3();

	RotationX = 0.0f;
	RotationY = 0.0f;
	RotationZ = 0.0f;
}

/*
	Initialises a new object

	@param aProgram The program object of the shaders the object will utilise
	@param aMesh The mesh of the object
*/
CObject::CObject(CProgram* aProgram, CMesh* aMesh) : Program(aProgram), Mesh(aMesh)
{
	Pivot = glm::vec3();
	Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	Location = glm::vec3();
	RotationX = 0.0f;
	RotationY = 0.0f;
	RotationZ = 0.0f;
}

/*
	Initialises a new object

	@param aProgram The program object of the shaders the object will utilise
	@param aMesh The mesh of the object
	@param aLoc Location of the object
	@param aScale Scale of the object
	@param aRot Rotation of the object
*/
CObject::CObject(CProgram* aProgram, CMesh* aMesh, glm::vec3 aLoc, glm::vec3 aScale, float aRot) : Program(aProgram), Mesh(aMesh), Location(aLoc), Scale(aScale), RotationZ(aRot)
{
	RotationX = 0.0f;
	RotationY = 0.0f;
}

/*
	Cleans up dynamic memory within CObject
*/
CObject::~CObject()
{
}

CProgram* CObject::GetProgram()
{
	return Program;
}

/*
	Returns the program ID

	@return A GLuint ID for the program
*/
GLuint CObject::GetProgramID()
{
	return Program->GetProgramID();
}

CMesh* CObject::GetMesh()
{
	return Mesh;
}

/*
	Assigns the uniforms to the shader in use
*/
void CObject::AssignUniforms(glm::mat4& pv)
{
	// Assign current time uniform
	glUniform1f(Program->GetUniform("currentTime"), TIMER::GetTotalSeconds());

	// Assign the PVM matrix uniform
	glm::mat4 model = GetModelMatrix();
	glm::mat4 pvm = pv * model;
	glUniformMatrix4fv(Program->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

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

	// Assign mat3 Uniforms
	for (std::map<GLuint, glm::mat3>::iterator it = UniformsMat3.begin(); it != UniformsMat3.end(); it++)
	{
		glUniformMatrix3fv(it->first, 1, GL_FALSE, glm::value_ptr(it->second));
	}

	// Assign mat4 Uniforms
	for (std::map<GLuint, glm::mat4>::iterator it = UniformsMat4.begin(); it != UniformsMat4.end(); it++)
	{
		glUniformMatrix4fv(it->first, 1, GL_FALSE, glm::value_ptr(it->second));
	}
	
	// Assign Texture Uniforms
	for (std::map<GLuint, GLuint&>::iterator it = UniformsTexturesR.begin(); it != UniformsTexturesR.end(); it++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, it->second);
		glUniform1i(it->first, i);
		i++;
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

	// Assign mat3 reference Uniforms
	for (std::map<GLuint, glm::mat3&>::iterator it = UniformsMat3R.begin(); it != UniformsMat3R.end(); it++)
	{
		glUniformMatrix3fv(it->first, 1, GL_FALSE, glm::value_ptr(it->second));
	}

	// Assign mat4 reference Uniforms
	for (std::map<GLuint, glm::mat4&>::iterator it = UniformsMat4R.begin(); it != UniformsMat4R.end(); it++)
	{
		glUniformMatrix4fv(it->first, 1, GL_FALSE, glm::value_ptr(it->second));
	}

	// Assign mat4 reference Uniforms
	for (std::map<GLuint, std::pair<int, float*>>::iterator it = UniformsfVec3Array.begin(); it != UniformsfVec3Array.end(); it++)
	{
		glUniform3fv(it->first, it->second.first, it->second.second);
	}
}

/*
	Draws the object
*/
void CObject::Draw(glm::mat4& pv)
{
	glUseProgram(GetProgramID());
	AssignUniforms(pv);
	Mesh->Draw();
	glUseProgram(0);

	for (int i = 0; i < (int)UniformsTextures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

/*
	Updates the model matrix and alarms
*/
void CObject::Update()
{
	float deltaSec = TIMER::GetDeltaSeconds();

	// Loop through all the alarms
	for (int i = 0; i < (int)Alarms.size(); ++i)
	{
		// Check if alarm is in use
		if (Alarms[i].first > 0)
		{
			// Remove the delta time
			Alarms[i].first -= deltaSec;
			if (Alarms[i].first <= 0)
			{
				Alarms[i].second(); // Execute function
				Alarms.erase(Alarms.begin() + i); // Delete the alarm
			}
		}
	}

	// Update loc rot scale for the render call
	UpdateModelMatrix();
}

/*
	Update an existing texture uniform or add a new one

	@param uniformName The name of the uniform to be set
	@param textureID The ID of the texture to be assigned
*/
void CObject::UpdateUniformTexture(std::string uniformName, GLuint textureID)
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
void CObject::UpdateUniformFloat(std::string uniformName, float aFloat)
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

void CObject::UpdateUniformInt(std::string uniformName, int aInt)
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
void CObject::UpdateUniformfVec2(std::string uniformName, glm::vec2 vec2)
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
void CObject::UpdateUniformfVec3(std::string uniformName, glm::vec3 vec3)
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

void CObject::UpdateUniformfVec4(std::string uniformName, glm::vec4 vec4)
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
void CObject::UpdateUniformiVec2(std::string uniformName, glm::ivec2 ivec2)
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

void CObject::UpdateUniformiVec3(std::string uniformName, glm::ivec3 ivec3)
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

void CObject::UpdateUniformiVec4(std::string uniformName, glm::ivec4 ivec4)
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

void CObject::UpdateUniformMat3(std::string uniformName, glm::mat3 mat3)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsMat3.insert_or_assign(loc, mat3);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CObject::UpdateUniformMat4(std::string uniformName, glm::mat4 mat4)
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

void CObject::UpdateUniformTextureR(std::string uniformName, GLuint& textureID)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsTexturesR.insert_or_assign(loc, textureID);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CObject::UpdateUniformFloatR(std::string uniformName, float& aFloat)
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

void CObject::UpdateUniformIntR(std::string uniformName, int& aInt)
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

void CObject::UpdateUniformfVec2R(std::string uniformName, glm::vec2& vec2)
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

void CObject::UpdateUniformfVec3R(std::string uniformName, glm::vec3& vec3)
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

void CObject::UpdateUniformfVec4R(std::string uniformName, glm::vec4& vec4)
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

void CObject::UpdateUniformiVec2R(std::string uniformName, glm::ivec2& ivec2)
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

void CObject::UpdateUniformiVec3R(std::string uniformName, glm::ivec3& ivec3)
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

void CObject::UpdateUniformiVec4R(std::string uniformName, glm::ivec4& ivec4)
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

void CObject::UpdateUniformMat3R(std::string uniformName, glm::mat3& mat3)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsMat3R.insert_or_assign(loc, mat3);
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}

void CObject::UpdateUniformMat4R(std::string uniformName, glm::mat4& mat4)
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

void CObject::UpdateUniformfVec3Array(std::string uniformName, int length, float* pointer)
{
	GLuint loc = glGetUniformLocation(Program->GetProgramID(), uniformName.c_str());

	if (loc != UINT_MAX)
	{
		UniformsfVec3Array.insert_or_assign(loc, std::make_pair(length, pointer));
	}
	else
	{
		std::cout << "WARNING: Uniform " + uniformName + " not found" << std::endl;
	}
}


/*
	Get the current model matrix

	@return The mat4 model matrix
*/
glm::mat4& CObject::GetModelMatrix()
{
	return ModelMatrix;
}

/*
	Updates the current model matrix based on location, rotation, scale etc.
*/
void CObject::UpdateModelMatrix()
{
	glm::mat4 pivot = glm::translate(glm::mat4(), -Pivot);
	ModelMatrix = GFX::CalculateModelMatrix(Location, Scale, RotationX, RotationY, RotationZ) * pivot;
}

/*
	Sets an alarm function to be called in x amount of seconds

	@param seconds The amount of seconds before the function is called
	@param aFunc Function to be called when time has passed
*/
void CObject::SetAlarm(float seconds, std::function<void()> aFunc)
{
	Alarms.push_back(std::make_pair(seconds, aFunc));
}