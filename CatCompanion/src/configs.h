#pragma once
#include <map>
#include <string>

class CConfigSettings
{
public:
	CConfigSettings();

	void SetDefaultFilename(std::string filename);
	void SetFileLocation(std::string filelocation);
	void AddDefaultFloatVariable(std::string name, float variable);
	void AddDefaultStringVariable(std::string name, std::string variable);

private:
	friend class CConfigs;

	std::string DefaultFilename;
	std::string FileLocation;

	std::map<std::string, float> DefaultFloatVariables;
	std::map<std::string, std::string> DefaultStringVariables;
};

class CConfigs
{
public:
	CConfigs(const CConfigSettings& settings);
	CConfigs(const CConfigSettings& settings,std::string filename);
	CConfigs(CConfigs* copyConfigs);

	bool DoesFloatExist(std::string name);
	bool DoesStringExist(std::string name);

	float GetFloatVariable(std::string name);
	std::string GetStringVariable(std::string name);

	void SetFloatVariable(std::string name, float value);
	void SetStringVariable(std::string name, std::string value);

	void SaveVariables();
	void SetFileName(std::string name);

	void Delete();

private:
	const CConfigSettings& Settings;

	// Variables
	std::string Filepath;
	std::string Filename;

	std::map<std::string, float> FloatVariables;
	std::map<std::string, std::string> StringVariables;

	void InheritSettings();
	void LoadVariables();
	void GenerateUniqueFilename();
};