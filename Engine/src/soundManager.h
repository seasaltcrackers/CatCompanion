#pragma once
#include <fmod.hpp>
#include <string>
#include <Map>

namespace SFX
{
	void Initialise();
	void Update();

	void GenerateSounds();

	FMOD::Sound* CreateSound(std::string fileName, int FmodMode);	
	void PlaySFX(FMOD::Sound* aSound);

	void CleanUp();
	FMOD::Sound* GetSound(std::string soundName);

	void SetEffectsVolume(float aVolume);
	void SetMusicVolume(float aVolume);
}

