#include <iostream>
#include <algorithm>
#include <filesystem>

#include "soundManager.h"
#include "helper.h"

const char SoundsFilePath[] = "Resources\\Audio\\"; // The file prefix while loading new audio files
static FMOD::System* AudioSystem;
static FMOD::SoundGroup* EffectsSG;
static FMOD::SoundGroup* MusicSG;
static std::map<std::string, FMOD::Sound*> Sounds;

/*
	Initialises the AudioSystem
*/
void SFX::Initialise()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&AudioSystem);
	result = AudioSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);

	IfThenStatement(result != FMOD_OK, std::cout << "Error Initialising Audio" << std::endl);

	result = AudioSystem->createSoundGroup("Effects", &EffectsSG);
	result = AudioSystem->createSoundGroup("Music", &MusicSG);


	IfThenStatement(result != FMOD_OK, std::cout << "Error Initialising Sound Group" << std::endl);
}

/*
	Updates the AudioSystem
*/
void SFX::Update()
{
	AudioSystem->update();
}

void SFX::GenerateSounds()
{
	IfThenReturn(!std::filesystem::exists(SoundsFilePath),);
	for (const auto& entry : std::filesystem::directory_iterator(SoundsFilePath))
	{
		std::string filename = entry.path().filename().replace_extension().u8string();
		std::string extension = entry.path().extension().u8string();

		if (extension == ".wav" || extension == ".mp3")
		{
			Sounds.insert({ filename, CreateSound(filename + extension, (extension == ".wav" ? FMOD_DEFAULT : FMOD_LOOP_NORMAL)) });
		}
	}
}

/*
	Creates a sound variable from a given audio file

	@param fileName The name of the sound file. (Assume prefixed with "Resources\\Audio\\")
	@param FmodMode The mode of the sound, e.g FMOD_DEFAULT or FMOD_LOOP_NORMAL
	@return A pointer to the new sound variable
*/
FMOD::Sound* SFX::CreateSound(std::string fileName, int FmodMode)
{
	FMOD_RESULT result;
	FMOD::Sound* tempSound;
	result = AudioSystem->createSound((SoundsFilePath + fileName).c_str(), FmodMode, 0, &tempSound);

	std::string fileType = fileName.substr(fileName.length() - 3, fileName.length());
	tempSound->setSoundGroup(fileType == "wav" ? EffectsSG : MusicSG);

	IfThenStatement(result != FMOD_OK, std::cout << "Error Creating Sound " << fileName << std::endl);
	return tempSound;
}

/*
	Plays a sound variable from a given audio file

	@param aSound The sound that should be played
*/
void SFX::PlaySFX(FMOD::Sound* aSound)
{
	FMOD_RESULT result;
	result = AudioSystem->playSound(aSound, 0, false, 0);
	IfThenStatement(result != FMOD_OK, std::cout << "Error Playing Sound " << std::endl);
}

/*
	Cleans up dynamic memory
*/
void SFX::CleanUp()
{
	std::for_each(Sounds.begin(), Sounds.end(), [](std::pair<const std::string, FMOD::Sound*>& it) { it.second->release(); });
	Sounds.clear();
	
	EffectsSG->release();
	MusicSG->release();

	AudioSystem->release();
}

FMOD::Sound* SFX::GetSound(std::string soundName)
{
	auto it = Sounds.find(soundName);
	if (it != Sounds.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "WARNING: Failed to find SOUND '" << soundName << "'" << std::endl;
		return nullptr;
	}
}

void SFX::SetEffectsVolume(float aVolume)
{
	EffectsSG->setVolume(aVolume);
}

void SFX::SetMusicVolume(float aVolume)
{
	MusicSG->setVolume(aVolume);
}