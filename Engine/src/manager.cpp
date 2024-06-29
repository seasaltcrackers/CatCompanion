#include <algorithm>

#include "manager.h"
#include "program.h"
#include "mesh.h"
#include "texture.h"
#include "graphics.h"
#include "soundManager.h"
#include "timer.h"
#include "time.h"
#include "camera.h"
#include "layer.h"
#include "font.h"
#include "room.h"
#include "layer.h"


void CManager::Initialise()
{
	srand(static_cast<unsigned int>(time(0)));

	CProgram::GeneratePrograms();
	CMesh::GenerateMeshes();
	CTexture::GenerateTextures();
	CFont::GenerateFonts();

	SFX::Initialise();
	SFX::GenerateSounds();

	GFX::Initialise();
	TIMER::Initialise();

	CCamera::SetScreenSize(ScreenWidth, ScreenHeight);
}

void CManager::Update()
{
	Rooms[CurrentRoomIndex]->Update();
	std::for_each(GlobalLayers.begin(), GlobalLayers.end(), [](ILayer* aLayer) { aLayer->Update(); });
	Rooms[CurrentRoomIndex]->UpdateCamera();
}

void CManager::Render()
{
	Rooms[CurrentRoomIndex]->Render();
	std::for_each(GlobalLayers.begin(), GlobalLayers.end(), [](ILayer* aLayer) { aLayer->Render(); });
}

void CManager::CleanUp()
{
	std::for_each(Rooms.begin(), Rooms.end(), [](CRoom* aRoom) { delete aRoom; });
	std::for_each(GlobalLayers.begin(), GlobalLayers.end(), [](ILayer* aLayer) { delete aLayer; });
	Rooms.clear();
	GlobalLayers.clear();

	CProgram::CleanUp();
	CMesh::CleanUp();
	CTexture::CleanUp();
	CFont::CleanUp();

	SFX::CleanUp();
}

void CManager::SetRoom(int index)
{
	CurrentRoomIndex = index;
	Rooms[index]->OnSelect();
}

void CManager::NextRoom()
{
	SetRoom((CurrentRoomIndex + 1) % Rooms.size());
}
