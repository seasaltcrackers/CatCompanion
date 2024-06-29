#include "configs.h"

#include "helper.h"

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

void CConfigs::InheritSettings()
{
	FloatVariables = Settings.DefaultFloatVariables;
	StringVariables = Settings.DefaultStringVariables;
	Filename = Settings.DefaultFilename;
}

CConfigs::CConfigs(const CConfigSettings& settings) :
	Settings(settings)
{
	InheritSettings();
	GenerateUniqueFilename();
	SaveVariables();
}

CConfigs::CConfigs(const CConfigSettings& settings, std::string filename) :
	Settings(settings)
{
	InheritSettings();
	
	Filename = filename;
	Filepath = Settings.FileLocation + filename + ".txt";
	Filename = Filename.substr(0, filename.find_last_of('-'));

	LoadVariables();
}

CConfigs::CConfigs(CConfigs* copyConfigs) :
	Settings(copyConfigs->Settings)
{
	FloatVariables = copyConfigs->FloatVariables;
	StringVariables = copyConfigs->StringVariables;
	Filename = copyConfigs->Filename;

	GenerateUniqueFilename();
	SaveVariables();
}

bool CConfigs::DoesFloatExist(std::string name)
{
	std::map<std::string, float>::iterator it = FloatVariables.find(name);
	return (it != FloatVariables.end());
}

bool CConfigs::DoesStringExist(std::string name)
{
	std::map<std::string, std::string>::iterator it = StringVariables.find(name);
	return (it != StringVariables.end());
}

float CConfigs::GetFloatVariable(std::string name)
{
	return FloatVariables[name];
}

std::string CConfigs::GetStringVariable(std::string name)
{
	return StringVariables[name];
}

void CConfigs::SetFloatVariable(std::string name, float value)
{
	FloatVariables.insert_or_assign(name, value);
}

void CConfigs::SetStringVariable(std::string name, std::string value)
{
	StringVariables.insert_or_assign(name, value);
}

void CConfigs::SaveVariables()
{
	std::ofstream ofs(Filepath, std::ofstream::trunc);

	for (auto it = FloatVariables.begin(); it != FloatVariables.end(); it++)
	{
		ofs << it->first << ": " << std::to_string(it->second) << "\n";
	}

	for (auto it = StringVariables.begin(); it != StringVariables.end(); it++)
	{
		ofs << it->first << ": " << it->second << "\n";
	}

	ofs.close();
}

void CConfigs::SetFileName(std::string name)
{
	Filename = name;
	std::remove(Filepath.c_str());
	GenerateUniqueFilename();
	SaveVariables();
}

void CConfigs::Delete()
{
	std::remove(Filepath.c_str());
}

void CConfigs::LoadVariables()
{
	std::ifstream infile(Filepath);

	std::string line;
	while (std::getline(infile, line))
	{
		std::vector<std::string> strs;
		HELPER::SplitString(line, strs, ':');
		
		if (strs.size() == 1)
		{
			strs.push_back("");
		}

		if (strs.size() == 2)
		{
			if (HELPER::IsFloat(strs[1]))
			{
				FloatVariables.insert_or_assign(strs[0], std::stof(strs[1]));
			}
			else
			{
				StringVariables.insert_or_assign(strs[0], HELPER::Trim(strs[1]));
			}
		}
	}
}

void CConfigs::GenerateUniqueFilename()
{
	std::string name = Filename == "" ? "DEFAULT" : Filename;
	std::string baseFilepath = Settings.FileLocation + name;

	if (!std::filesystem::exists(baseFilepath + ".txt"))
	{
		Filepath = baseFilepath + ".txt";
	}
	else
	{
		int index = 1;
		while (std::filesystem::exists(baseFilepath + "-" + std::to_string(index) + ".txt")) { ++index; }
		Filepath = baseFilepath + "-" + std::to_string(index) + ".txt";
	}
}

CConfigSettings::CConfigSettings()
{
	DefaultFilename = "";
	FileLocation = "";
}

void CConfigSettings::SetDefaultFilename(std::string filename)
{
	DefaultFilename = filename;
}

void CConfigSettings::SetFileLocation(std::string filelocation)
{
	FileLocation = filelocation;
}

void CConfigSettings::AddDefaultFloatVariable(std::string name, float variable)
{
	DefaultFloatVariables.insert({ name, variable });
}

void CConfigSettings::AddDefaultStringVariable(std::string name, std::string variable)
{
	DefaultStringVariables.insert({ name, variable });
}
