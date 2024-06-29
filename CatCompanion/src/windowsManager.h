#pragma once
#include <map>
#include <vector>
#include <Windows.h>

#include "monitorManager.h"

class CFont;

struct Platform
{
	float Left;
	float Right;
	float Height;
	HWND Hwnd;
};

class CWindowsManager
{
public:
	static CWindowsManager& GetInstance();
	static void DestroyInstance();
	
	~CWindowsManager();

	void Update();
	void Render();

	std::vector<Platform>& GetPlatforms();
	bool GetPlatformsFromWindow(HWND hwnd, std::vector<Platform>& platforms);

	bool GetWindowBoundryLocalCoords(HWND hWnd, RECT& bounds);
	bool GetWindowBoundry(HWND hWnd, RECT& bounds);
	std::string GetWindowName(HWND hWnd);

private:
	static CWindowsManager* Instance;

	CMonitorManager& MM;
	CFont* Font;

	std::map<HWND, RECT> VisibleWindowsBounds;
	std::vector<HWND> VisibleWindows;

	std::vector<Platform> Platforms;
	std::map<HWND, std::vector<Platform>> WindowPlatforms;

	CWindowsManager();
	CWindowsManager(CWindowsManager const&);

	void CalculatePlatforms();
	void CalculatePlatform(Platform platform, std::vector<RECT>& bounds, int startIndex = 0);
	static BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lparam);
};

