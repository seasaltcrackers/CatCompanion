#include <Windows.h>
#include <strsafe.h>
#include <dwmapi.h>
#include <thread>
#pragma comment(lib, "dwmapi.lib")

#include "scene.h"
#include "program.h"
#include "object.h"
#include "helper.h"
#include "sprite.h"
#include "input.h"
#include "mesh.h"
#include "timer.h"
#include "coreTilemap.h"
#include "graphics.h"
#include "detailsWindow.h"
#include "camera2D.h"
#include "room.h"
#include "instanceLayer.h"
#include "windowsManager.h"
#include "monitorManager.h"
#include "font.h"
#include "cat.h"
#include "focusManager.h"
#include "exitObject.h"
#include "configs.h"
#include "catPropertiesWindow.h"
#include "texturedParticleLayer.h"
#include "catManager.h"
#include "CatWindow.h"

#define MAX_TITLE_LEN 100

HWND CScene::hWnd;
bool CScene::IsOpen;

//CDetailsWindow* DetailsWindow;

CScene::CScene(HWND aHwnd, unsigned int scrWidth, unsigned int scrHeight) :
	WM(CWindowsManager::GetInstance()), MM(CMonitorManager::GetInstance()), FM(CFocusManager::GetInstance())
{
	hWnd = aHwnd;
	ScreenWidth = scrWidth;
	ScreenHeight = scrHeight;
}

void CScene::Update()
{
	TIMER::Update(); // Update Delta and Total times
	GINPUT::UpdateGlobal(); // Update Keystates and Mouse Locations

	// Update the window handles
	//std::thread wmThread(std::bind(&CWindowsManager::Update, &WM));
	//std::thread mmThread(std::bind(&CMonitorManager::Update, &MM));
	WM.Update();
	MM.Update();


	// Update all the objects
	CManager::Update();
	//DetailsWindow->Update();

	FM.Update();

	// F3 has been pressed
	if (F3DebugEnabled && GINPUT::GetKeyInputState(114) == InputState::INPUT_DOWN_FIRST) { DebugMode = !DebugMode; }

	//wmThread.join();
	//mmThread.join();

	Sleep(1);
}

void CScene::Render()
{
	IfThenStatement(DebugMode, WM.Render());
	CManager::Render();
	//DetailsWindow->Draw();

	//glm::ivec2 aLoc = GINPUT::GetMouseLocation();
	//GFX::DrawString(CCamera::GetProjViewMatrix(), CFont::GetFont("AcknowledgeTTSmall"), "  " + HELPER::ToStringVec2(GINPUT::GetMouseLocation()), aLoc, glm::ivec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);

	glFinish();
}

void CScene::CleanUp()
{
	CManager::CleanUp();

	IsRunning = false;
	IfThenStatement(WindowsProcessorThread.joinable(), WindowsProcessorThread.join());

	/*delete DetailsWindow;
	DetailsWindow = nullptr;*/

	CWindowsManager::DestroyInstance();
	CMonitorManager::DestroyInstance();

	//CCoreTilemap::DestroyTileTemplates();
}

void CScene::OnFileDropped(HICON icon)
{
	glm::ivec2 mouseLocation = GINPUT::GetMouseLocation();
	CCat* targetCat = nullptr;

	for (CCat* cat : Cats)
	{
		if (cat->IsInBounds(mouseLocation))
		{
			targetCat = cat;
			break;
		}
	}

	if (targetCat != nullptr)
	{
		targetCat->OnFileDropped(icon);
	}
}

void CScene::OpenCatPropertiesWindow(CCat* cat)
{
	PropertyWindow->SetVisible(cat);
}

void CScene::WindowsProcessor()
{
	float sleepTime = 1000 / 60;

	while (IsRunning)
	{
		WM.Update();
		Sleep(DWORD(sleepTime));
	}
}


void CScene::Initialise()
{
	//SetWinEventHook();

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	CManager::Initialise();

	F3DebugEnabled = CGlobalVariables::ConfigSetting->GetStringVariable("f3-debug") == "enabled";

	std::shared_ptr<CCamera2D> mainCamera = std::shared_ptr<CCamera2D>(new CCamera2D(glm::vec2()));

	//CCoreTilemap::GenerateTileTemplates();

	CRoom* room = new CRoom(mainCamera);
	Rooms.push_back(room);

	CMonitorManager& MM = CMonitorManager::GetInstance();
	std::vector<HMONITOR>& Monitors = MM.GetMonitors();

	//std::vector<CCoreTilemap*> tilemaps;
	//std::for_each(Monitors.begin(), Monitors.end(), [&tilemaps, &MM](HMONITOR mon) { tilemaps.push_back(new CCoreTilemap((int)mon)); });
	////tilemaps.push_back(new CCoreTilemap(Monitors[0]));
	
	MonitorInfo tempInfo = MM.GetMonitorDetail((int)Monitors[0]);
	//glm::ivec2 tempLoc = MM.DesktopToWindow(glm::ivec2(tempInfo.BoundryNTB.left + tempInfo.Width * 0.5f, tempInfo.BoundryNTB.top + tempInfo.Height * 0.5f));
	//DetailsWindow = new CDetailsWindow(tilemaps, tempLoc);
	//room->Layers.insert(room->Layers.end(), tilemaps.begin(), tilemaps.end());


	CInstanceLayer* instanceLayer = new CInstanceLayer();
	ParticlesLayer = new CTexturedParticleLayer();
	CCatManager* catManager = new CCatManager(this);

	room->Layers.push_back(catManager);
	room->Layers.push_back(instanceLayer);
	room->Layers.push_back(ParticlesLayer);

	
	PropertyWindow = new CCatPropertiesWindow(catManager);
	instanceLayer->Objects.push_back(PropertyWindow);

	instanceLayer->Objects.push_back(new CExitObject());


	SetRoom(0);
}