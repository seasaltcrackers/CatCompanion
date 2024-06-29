#pragma once
#include  <Windows.h>
#include <thread>

#include "manager.h"

class CTexturedParticleLayer;
class CCatPropertiesWindow;
class CWindowsManager;
class CMonitorManager;
class CFocusManager;
class CCat;

class CScene :
	public CManager
{
public:
	static HWND hWnd;
	static bool IsOpen;

	CScene(HWND aHwnd, unsigned int scrWidth, unsigned int scrHeight);

	void Initialise();
	void Update();
	void Render();
	void CleanUp();

	void OnFileDropped(HICON icon);
	void OpenCatPropertiesWindow(CCat* cat);

	CTexturedParticleLayer* ParticlesLayer;

private:
	CWindowsManager& WM;
	CMonitorManager& MM;
	CFocusManager& FM;

	std::vector<CCat*> Cats;
	CCatPropertiesWindow* PropertyWindow;

	bool DebugMode = false;
	bool F3DebugEnabled = false;

	bool IsRunning = true;
	std::thread WindowsProcessorThread;

	void WindowsProcessor();
};

