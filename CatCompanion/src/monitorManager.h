#pragma once
#include <glm.hpp>
#include <windows.h>
#include <map>
#include <vector>
#include <mutex>
#include <set>

struct MonitorInfo
{
	RECT BoundryNTB;
	RECT BoundryTB;

	int TaskBarHeight;
	
	int Width;
	int Height;
};

class CMonitorManager
{
public:
	static CMonitorManager& GetInstance();
	static void DestroyInstance();

	~CMonitorManager();

	void Update();
	
	//glm::ivec2 DesktopToWindow(glm::ivec2 aVec);
	glm::vec2 DesktopToWindow(glm::vec2 aVec);

	//glm::ivec2 WindowToDesktop(glm::ivec2 aVec);
	glm::vec2 WindowToDesktop(glm::vec2 aVec);

	RECT& GetCombinedRect();
	MonitorInfo& GetMonitorDetail(int aMonitor);
	std::vector<HMONITOR>& GetMonitors();


private:
	static CMonitorManager* Instance;

	glm::vec2 MouseLoc;

	RECT Combined;
	std::map<int, MonitorInfo> MonitorDetails;
	std::vector<HMONITOR> Monitors;
	std::mutex MonitorDetailsLock;
	std::set<int> ValidMonitors;

	CMonitorManager() {};
	CMonitorManager(CMonitorManager const&) {};

	void InitialiseValidMonitors();

	static int CurrentMonitorID;
	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
};

