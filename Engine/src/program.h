#pragma once
#include <glew.h>
#include <string>
#include <map>

class CProgram
{
public:
	CProgram();
	CProgram(GLuint aProgramID);

	GLuint GetProgramID();
	GLuint GetUniform(std::string aName);

	static CProgram* GenerateProgramVF(const std::string vertexShaderFilename, const std::string fragmentShaderFilename);
	static CProgram* GenerateProgramVGF(const std::string vertexShaderFilename, const std::string geometryShaderFilename, const std::string fragmentShaderFilename);
	static CProgram* GenerateProgramVTF(const std::string vertexShaderFilename, const std::string tessCtrlShaderFilename, const std::string tessEvalShaderFilename, const std::string fragmentShaderFilename);
	static CProgram* GenerateProgramVTGF(const std::string vertexShaderFilename, const std::string tessCtrlShaderFilename, const std::string tessEvalShaderFilename, const std::string geometryShaderFilename, const std::string fragmentShaderFilename);
	static CProgram* GenerateProgramC(const std::string computeShaderFilename);

	static void CleanUp();
	static void GeneratePrograms();
	static CProgram* GetProgram(std::string programName);

private:
	GLuint ProgramID;
	std::map<std::string, GLuint> UniformIDs;

	static GLuint CreateShader(GLenum shaderType, const char* filename);
	static std::string ReadShaderFile(const char* filename);
	static void PrintErrorDetails(bool isShader, GLuint id, const char* name);

	static std::string GetFilepath();
	static std::map<std::string, CProgram*> Programs; // Contains all the available Programs
};


