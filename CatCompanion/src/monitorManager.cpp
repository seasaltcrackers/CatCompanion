#include "monitorManager.h"
#include "input.h"
#include "CatWindow.h"
#include "configs.h"
#include "helper.h"

CMonitorManager* CMonitorManager::Instance = nullptr;
int CMonitorManager::CurrentMonitorID = 0;

CMonitorManager& CMonitorManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new CMonitorManager();
		Instance->InitialiseValidMonitors();
		Instance->Update();

		//SetRectEmpty(&Instance->Combined);
		//Instance->Monitors.clear();
		//EnumDisplayMonitors(NULL, NULL, MonitorEnum, NULL);
	}

	return *Instance;
}

void CMonitorManager::DestroyInstance()
{
	delete Instance;
	Instance = nullptr;
}

CMonitorManager::~CMonitorManager()
{
}

void CMonitorManager::Update()
{
	// Update Mouse Position:
	glm::vec2 temp = GINPUT::GetMouseLocation();
	GINPUT::MouseMove(MouseLoc.x, MouseLoc.y);
	MouseLoc = DesktopToWindow(temp);
	GINPUT::MouseMove(MouseLoc.x, MouseLoc.y);

	SetRectEmpty(&Combined);
	Monitors.clear();
	CurrentMonitorID = 0;

	EnumDisplayMonitors(NULL, NULL, MonitorEnum, NULL);
}

//glm::ivec2 CMonitorManager::DesktopToWindow(glm::ivec2 aVec)
//{
//	return aVec - glm::ivec2(Combined.left, Combined.top);
//}

glm::vec2 CMonitorManager::DesktopToWindow(glm::vec2 aVec)
{
	return aVec - glm::vec2(Combined.left, Combined.top);
}

//glm::ivec2 CMonitorManager::WindowToDesktop(glm::ivec2 aVec)
//{
//	return aVec + glm::ivec2(Combined.left, Combined.top);
//}

glm::vec2 CMonitorManager::WindowToDesktop(glm::vec2 aVec)
{
	return aVec + glm::vec2(Combined.left, Combined.top);
}

RECT& CMonitorManager::GetCombinedRect()
{
	return Combined;
}

MonitorInfo& CMonitorManager::GetMonitorDetail(int aMonitor)
{
	MonitorDetailsLock.lock();
	MonitorInfo& info = MonitorDetails[aMonitor];
	MonitorDetailsLock.unlock();
	return info;
}

std::vector<HMONITOR>& CMonitorManager::GetMonitors()
{
	return Monitors;
}

void CMonitorManager::InitialiseValidMonitors()
{
	std::string monitors = CGlobalVariables::ConfigSetting->GetStringVariable("monitors");

	if (monitors._Starts_with("manual"))
	{
		const int manualSize = std::string("manual").size();
		std::string monitorList = monitors.substr(manualSize, monitors.size() - manualSize);

		monitorList.erase(std::remove(monitorList.begin(), monitorList.end(), '{'), monitorList.end());
		monitorList.erase(std::remove(monitorList.begin(), monitorList.end(), '}'), monitorList.end());

		std::vector<std::string> split;
		HELPER::SplitString(monitorList, split, ','); 

		for (std::string str : split)
		{
			ValidMonitors.insert(std::stoi(str));
		}
	}
}

BOOL CMonitorManager::MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	CMonitorManager& pThis = GetInstance();

	if (pThis.ValidMonitors.size() > 0 && pThis.ValidMonitors.find(CurrentMonitorID) == pThis.ValidMonitors.end())
	{
		CurrentMonitorID++;
		return TRUE;
	}
	
	CurrentMonitorID++;

	MONITORINFO info = {};
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMon, &info);
	int tempTaskBarHeight = info.rcMonitor.bottom - info.rcWork.bottom;

	
	pThis.MonitorDetailsLock.lock();
	pThis.MonitorDetails.insert_or_assign((int)hMon,  MonitorInfo{ *lprcMonitor, info.rcWork, tempTaskBarHeight, lprcMonitor->right - lprcMonitor->left, lprcMonitor->bottom - lprcMonitor->top });
	pThis.MonitorDetailsLock.unlock();
	
	pThis.Monitors.push_back(hMon);
	UnionRect(&pThis.Combined, &pThis.Combined, lprcMonitor);
	return TRUE;
}
