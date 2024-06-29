#include <algorithm>
#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include "program.h"
#include "helper.h"

std::map<std::string, CProgram*> CProgram::Programs; // Contains all the available Programs


// Map of all the existing programs and shaders
static std::map<std::string, GLuint> ExistingPrograms;
static std::map<std::string, GLuint> ExistingShaders;

/*
	Initialises an empty program
*/
CProgram::CProgram()
{
	ProgramID = 0;
}

/*
	Create a program with the given ID

	@param aProgramID The ID of the program
*/
CProgram::CProgram(GLuint aProgramID) : ProgramID(aProgramID)
{
	ProgramID = aProgramID;
}

/*
	Get the program ID

	@return A GLuint of the program ID
*/
GLuint CProgram::GetProgramID()
{
	return ProgramID;
}

/*
	Find or retrieve a uniform within the given program

	@param aName The string name of the uniform
	@return The GLuint ID of the uniform with the given name
*/
GLuint CProgram::GetUniform(std::string aName)
{
	std::map<std::string, GLuint>::iterator temp = UniformIDs.find(aName);
	if (temp != UniformIDs.end())
	{
		return (*temp).second;
	}
	else
	{
		return UniformIDs.insert({ aName, glGetUniformLocation(ProgramID, aName.c_str()) }).second;
	}
}

CProgram* CProgram::GenerateProgramVF(const std::string vertexShaderFilename, const std::string fragmentShaderFilename)
{
	GLuint programID = 0;
	std::string programName = vertexShaderFilename + fragmentShaderFilename;

	if (ExistingPrograms.find(programName) == ExistingPrograms.end())
	{
		programID = glCreateProgram();
		glAttachShader(programID, CreateShader(GL_FRAGMENT_SHADER, (GetFilepath() + fragmentShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_VERTEX_SHADER, (GetFilepath() + vertexShaderFilename).c_str()));
		glLinkProgram(programID);

		ExistingPrograms.insert({ programName, programID });
	}
	else
	{
		programID = ExistingPrograms[programName];
	}

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		PrintErrorDetails(false, programID, programName.c_str());
	}

	return new CProgram(programID);
}

CProgram* CProgram::GenerateProgramVGF(const std::string vertexShaderFilename, const std::string geometryShaderFilename, const std::string fragmentShaderFilename)
{
	GLuint programID = 0;

	std::string programName = vertexShaderFilename + geometryShaderFilename + fragmentShaderFilename;

	if (ExistingPrograms.find(programName) == ExistingPrograms.end())
	{
		programID = glCreateProgram();
		glAttachShader(programID, CreateShader(GL_FRAGMENT_SHADER, (GetFilepath() + fragmentShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_GEOMETRY_SHADER, (GetFilepath() + geometryShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_VERTEX_SHADER, (GetFilepath() + vertexShaderFilename).c_str()));
		glLinkProgram(programID);

		ExistingPrograms.insert({ programName, programID });
	}
	else
	{
		programID = ExistingPrograms[programName];
	}

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		PrintErrorDetails(false, programID, programName.c_str());
	}

	return new CProgram(programID);
}

CProgram* CProgram::GenerateProgramVTF(const std::string vertexShaderFilename, const std::string tessCtrlShaderFilename, const std::string tessEvalShaderFilename, const std::string fragmentShaderFilename)
{
	GLuint programID = 0;

	std::string programName = vertexShaderFilename + tessCtrlShaderFilename + tessEvalShaderFilename + fragmentShaderFilename;

	if (ExistingPrograms.find(programName) == ExistingPrograms.end())
	{
		programID = glCreateProgram();
		glAttachShader(programID, CreateShader(GL_FRAGMENT_SHADER, (GetFilepath() + fragmentShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_TESS_CONTROL_SHADER, (GetFilepath() + tessCtrlShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_TESS_EVALUATION_SHADER, (GetFilepath() + tessEvalShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_VERTEX_SHADER, (GetFilepath() + vertexShaderFilename).c_str()));
		glLinkProgram(programID);

		ExistingPrograms.insert({ programName, programID });
	}
	else
	{
		programID = ExistingPrograms[programName];
	}

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		PrintErrorDetails(false, programID, programName.c_str());
	}

	return new CProgram(programID);
}

CProgram* CProgram::GenerateProgramVTGF(const std::string vertexShaderFilename, const std::string tessCtrlShaderFilename, const std::string tessEvalShaderFilename, const std::string geometryShaderFilename, const std::string fragmentShaderFilename)
{
	GLuint programID = 0;

	std::string programName = vertexShaderFilename + tessCtrlShaderFilename + tessEvalShaderFilename + geometryShaderFilename + fragmentShaderFilename;

	if (ExistingPrograms.find(programName) == ExistingPrograms.end())
	{
		programID = glCreateProgram();
		glAttachShader(programID, CreateShader(GL_FRAGMENT_SHADER, (GetFilepath() + fragmentShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_TESS_CONTROL_SHADER, (GetFilepath() + tessCtrlShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_TESS_EVALUATION_SHADER, (GetFilepath() + tessEvalShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_GEOMETRY_SHADER, (GetFilepath() + geometryShaderFilename).c_str()));
		glAttachShader(programID, CreateShader(GL_VERTEX_SHADER, (GetFilepath() + vertexShaderFilename).c_str()));
		glLinkProgram(programID);

		ExistingPrograms.insert({ programName, programID });
	}
	else
	{
		programID = ExistingPrograms[programName];
	}

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		PrintErrorDetails(false, programID, programName.c_str());
	}

	return new CProgram(programID);
}

CProgram* CProgram::GenerateProgramC(const std::string computeShaderFilename)
{
	GLuint programID = 0;

	std::string programName = computeShaderFilename;

	if (ExistingPrograms.find(programName) == ExistingPrograms.end())
	{
		programID = glCreateProgram();
		glAttachShader(programID, CreateShader(GL_COMPUTE_SHADER, (GetFilepath() + programName).c_str()));
		glLinkProgram(programID);

		ExistingPrograms.insert({ programName, programID });
	}
	else
	{
		programID = ExistingPrograms[programName];
	}

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		PrintErrorDetails(false, programID, programName.c_str());
	}

	return new CProgram(programID);
}

/*
	Delete all the dynamic memory for the Programs map
*/
void CProgram::CleanUp()
{
	ExistingPrograms.clear();
	ExistingShaders.clear();

	std::for_each(Programs.begin(), Programs.end(), [](std::pair<const std::string, CProgram*>& it) { delete it.second; });
	Programs.clear();
}

void CProgram::GeneratePrograms()
{
	std::vector<std::string> vsShaders;
	std::vector<std::string> gsShaders;
	std::vector<std::string> fsShaders;
	std::vector<std::string> tcsShaders;
	std::vector<std::string> tesShaders;
	std::vector<std::string> compShaders;

	for (const auto& entry : std::filesystem::directory_iterator(GetFilepath()))
	{
		std::string filename = entry.path().filename().replace_extension().u8string();
		std::string extension = entry.path().extension().u8string();

		(extension == ".vs" ? vsShaders : extension == ".gs" ? gsShaders : extension == ".tes" ? tesShaders : extension == ".tcs" ? tcsShaders : extension == ".cs" ? compShaders : fsShaders).push_back(filename);
	}


	for (int i = 0; i < (int)compShaders.size(); ++i)
	{
		Programs.insert({ compShaders[i], GenerateProgramC(compShaders[i] + ".cs") });
		std::cout << "Asset Shader { " << compShaders[i] + ".cs }" << std::endl;
	}

	for (int i = 0; i < (int)fsShaders.size(); ++i)
	{
		auto itVS = std::find(vsShaders.begin(), vsShaders.end(), fsShaders[i]);
		auto itGS = std::find(gsShaders.begin(), gsShaders.end(), fsShaders[i]);
		auto itTCS = std::find(tcsShaders.begin(), tcsShaders.end(), fsShaders[i]);
		auto itTES = std::find(tesShaders.begin(), tesShaders.end(), fsShaders[i]);

		std::string vsFileName = "";

		if (itVS == vsShaders.end())
		{
			vsFileName = ((fsShaders[i]).substr(0, 2) == "BL" ? "BasicLighting" : "Basic");
		}
		else
		{
			vsFileName = (*itVS);
		}

		if (itGS == gsShaders.end())
		{
			if (itTCS != tcsShaders.end() && itTES != tesShaders.end())
			{
				std::cout << "Asset Shader { " << vsFileName + ".vs, " + (*itTCS) + ".tcs, " + (*itTES) + ".tes, " + fsShaders[i] + ".fs } has been loaded" << std::endl;
				Programs.insert({ fsShaders[i], GenerateProgramVTF(vsFileName + ".vs", (*itTCS) + ".tcs", (*itTES) + ".tes", fsShaders[i] + ".fs") });
			}
			else
			{
				std::cout << "Asset Shader { " << vsFileName + ".vs, " + fsShaders[i] + ".fs } has been loaded" << std::endl;
				Programs.insert({ fsShaders[i], GenerateProgramVF(vsFileName + ".vs", fsShaders[i] + ".fs") });
			}

		}
		else
		{
			if (itTCS != tcsShaders.end() && itTES != tesShaders.end())
			{
				std::cout << "Asset Shader { " << vsFileName + ".vs, " + (*itTCS) + ".tcs, " + (*itTES) + ".tes, " + (*itGS) + ".gs, " + fsShaders[i] + ".fs } has been loaded" << std::endl;
				Programs.insert({ fsShaders[i], GenerateProgramVTGF(vsFileName + ".vs", (*itTCS) + ".tcs", (*itTES) + ".tes", (*itGS) + ".gs", fsShaders[i] + ".fs") });
			}
			else
			{
				std::cout << "Asset Shader { " << vsFileName + ".vs, " + (*itGS) + ".gs, " + fsShaders[i] + ".fs } has been loaded" << std::endl;
				Programs.insert({ fsShaders[i], GenerateProgramVGF(vsFileName + ".vs", (*itGS) + ".gs", fsShaders[i] + ".fs") });
			}
		}
	}
}

CProgram* CProgram::GetProgram(std::string programName)
{
	auto it = Programs.find(programName);
	if (it != Programs.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "WARNING: Failed to find PROGRAM '" << programName << "'" << std::endl;
		return nullptr;
	}
}

/*
	Create a shader ID based on a type and filename

	@param shaderType GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	@param filename The file path of the shader
	@return The GLuint ID of the shader
*/
GLuint CProgram::CreateShader(GLenum shaderType, const char* filename)
{
	GLuint shaderID = 0;

	if (ExistingShaders.find(filename) == ExistingShaders.end())
	{
		std::string aFile = ReadShaderFile(filename);
		const char* source = aFile.c_str();
		const GLint sourceSize = aFile.size();

		shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &source, &sourceSize);
		glCompileShader(shaderID);

		ExistingShaders.insert({ filename, shaderID });
	}
	else
	{
		shaderID = ExistingShaders[filename];
	}

	// Check for errors
	int compile_result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
	if (compile_result == GL_FALSE)
	{
		PrintErrorDetails(true, shaderID, filename);
		return 0;
	}
	return shaderID;
}

/*
	Creates a string of a shader file

	@param filename The file path of the shader
	@return String containing the entire shader text
*/
std::string CProgram::ReadShaderFile(const char* filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

/*
	Prints out any errors with loading a shader

	@param isShader true if it's a shader, false if it is a program
	@param id The shader or program ID
	@param name The name of the shader/program
*/
void CProgram::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader == true) ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength) : glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	if (infoLogLength != 0)
	{
		std::vector<char> log(infoLogLength);

		// Retrieve the log info and populate log variable
		(isShader == true) ? glGetShaderInfoLog(id, infoLogLength, NULL, &log[0]) : glGetProgramInfoLog(id, infoLogLength, NULL, &log[0]);
		std::cout << "Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
		std::cout << &log[0] << std::endl;
	}
	else
	{
		std::cout << "Unknown Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
	}
}

std::string CProgram::GetFilepath()
{
	return (std::filesystem::current_path().parent_path()).string() + "\\Resources\\Shaders\\";
}
