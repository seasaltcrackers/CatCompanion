#include <dwmapi.h>
#include <glm.hpp>
#include <glew.h>
#include <algorithm>

#include "windowsManager.h"
#include "graphics.h"
#include "camera.h"
#include "scene.h"
#include "helper.h"
#include "font.h"
#include "input.h"

CWindowsManager* CWindowsManager::Instance = nullptr;

CWindowsManager& CWindowsManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new CWindowsManager();
		Instance->Update();
	}

	return *Instance;
}

void CWindowsManager::DestroyInstance()
{
	delete Instance;
	Instance = nullptr;
}

CWindowsManager::~CWindowsManager()
{
}

void CWindowsManager::Update()
{
	VisibleWindows.clear();
	VisibleWindowsBounds.clear();
	EnumWindows(EnumWindowCallback, NULL);
	CalculatePlatforms();
}

void CWindowsManager::Render()
{
	glm::mat4 pv = CCamera::GetProjViewMatrix();

	for (Platform& p : Platforms)
	{
		GFX::DrawLine(pv, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(p.Left, p.Height), glm::vec2(p.Right, p.Height), 5.0f);
	}

	if (Font == nullptr) { Font = CFont::GetFont("arialSmall"); }

	//glm::mat4 pv = CCamera::GetProjViewMatrix();
	glm::vec4 overwriteCol = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 windowCol = glm::vec4(1.0f, 0.0f, 0.0f, 0.2f);
	glm::vec4 textCol = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = VisibleWindows.size() - 1; i >= 0; --i)
	{
		HWND hWnd = VisibleWindows[i];
		RECT r = VisibleWindowsBounds[hWnd];

		glm::vec2 topLeft1 = MM.DesktopToWindow(glm::vec2(r.left, r.top));
		glm::vec2 botRight1 = MM.DesktopToWindow(glm::vec2(r.right, r.bottom));

		glm::vec2 topLeft2 = MM.DesktopToWindow(glm::vec2(r.left, r.top - 10));
		glm::vec2 botRight2 = MM.DesktopToWindow(glm::vec2(r.right, r.top));

		// Overwrite anything underneath the window:
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		GFX::DrawRectangle(pv, overwriteCol, topLeft1, botRight1 - topLeft1);

		// Draw something on top:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//GFX::DrawRectangle(pv, windowCol, topLeft2, botRight2 - topLeft2);
		GFX::DrawRectangle(pv, windowCol, topLeft1, botRight1 - topLeft1);

		std::string name = GetWindowName(hWnd);
		glm::ivec2 location = topLeft1 + 0.5f * (botRight1 - topLeft1) - glm::vec2(Font->TextBounds(name, 1.0f) / 2);
		GFX::DrawString(pv, Font, name, location, textCol);
	}
}

/*
std::vector<HWND>& CWindowsManager::GetCollisionWindows()
{
	return CollisionWindows;
}*/

std::vector<Platform>& CWindowsManager::GetPlatforms()
{
	return Platforms;
}

bool CWindowsManager::GetPlatformsFromWindow(HWND hwnd, std::vector<Platform>& platforms)
{
	auto it = WindowPlatforms.find(hwnd);
	if (it != WindowPlatforms.end())
	{
		platforms = it->second;
		return true;
	}
	else
	{
		HELPER::PrintToOutput("WARNING: HWND does not exist");
		return false;
	}
}

bool CWindowsManager::GetWindowBoundryLocalCoords(HWND hWnd, RECT& bounds)
{
	auto it = VisibleWindowsBounds.find(hWnd);

	if (it != VisibleWindowsBounds.end())
	{
		bounds = VisibleWindowsBounds[hWnd];

		glm::ivec2 topLeft = MM.DesktopToWindow({ bounds.left, bounds.top });
		glm::ivec2 botRight = MM.DesktopToWindow({ bounds.right, bounds.bottom });

		bounds.left = topLeft.x;
		bounds.top = topLeft.y;

		bounds.right = botRight.x;
		bounds.bottom = botRight.y;

		return true;
	}
	else
	{
		return false;
	}
}

bool CWindowsManager::GetWindowBoundry(HWND hWnd, RECT& bounds)
{
	auto it = VisibleWindowsBounds.find(hWnd);

	if (it != VisibleWindowsBounds.end())
	{
		bounds = VisibleWindowsBounds[hWnd];
		return true;
	}
	else
	{
		return false;
	}
}

std::string CWindowsManager::GetWindowName(HWND hWnd)
{
	// Get the windows title
	int length = GetWindowTextLength(hWnd);
	wchar_t* buffer = new wchar_t[length + 1];
	GetWindowText(hWnd, buffer, length + 1);
	std::wstring text = buffer;
	std::string windowTitle(text.begin(), text.end());
	delete[] buffer;

	return windowTitle;
}

CWindowsManager::CWindowsManager() : MM(CMonitorManager::GetInstance()) {}
CWindowsManager::CWindowsManager(CWindowsManager const&) : MM(CMonitorManager::GetInstance()) {}

void CWindowsManager::CalculatePlatforms()
{
	// Clear the current platforms
	Platforms.clear();
	WindowPlatforms.clear();

	std::vector<RECT> bounds;

	// Loop through each window backwards
	//for (int i = VisibleWindows.size() - 1; i >= 0; i--)
	for (int i = 0; i < VisibleWindows.size(); ++i)
	{
		// Get the boundry of the window
		RECT b;
		GetWindowBoundry(VisibleWindows[i], b);

		// Create a platform for it and run it across all previous boundries
		Platform p = Platform{ (float)b.left, (float)b.right, (float)b.top, VisibleWindows[i] };
		CalculatePlatform(p, bounds);
		
		// Add this window to the list of windows
		bounds.push_back(b);
	}
}

void CWindowsManager::CalculatePlatform(Platform p, std::vector<RECT>& bounds, int startIndex)
{
	for (int i = startIndex; i < bounds.size(); ++i)
	{
		RECT b = bounds[i];

		if (b.bottom > p.Height && b.top < p.Height)
		{
			if (b.left <= p.Left && b.right >= p.Right)
			{
				// Platform is completely covered
				return;
			}
			else if (b.left > p.Left && b.right < p.Right)
			{
				// Platform has been split
				Platform resultA = Platform{ p.Left, (float)b.left, p.Height, p.Hwnd };
				Platform resultB = Platform{ (float)b.right, p.Right, p.Height, p.Hwnd };

				CalculatePlatform(resultA, bounds, i + 1);
				CalculatePlatform(resultB, bounds, i + 1);
				return;
			}
			else if (b.right > p.Left && b.left < p.Left)
			{
				// Platform has been shrunk to the right
				Platform result = Platform{ (float)b.right, p.Right, p.Height, p.Hwnd };
				CalculatePlatform(result, bounds, i + 1);
				return;
			}
			else if (b.left < p.Right && b.right > p.Right)
			{
				// Platform has been shrunk to the left
				Platform result = Platform{ p.Left, (float)b.left, p.Height, p.Hwnd };
				CalculatePlatform(result, bounds, i + 1);
				return;
			}
		}
	}

	// Convert to cat window coordinates
	glm::vec2 left = MM.DesktopToWindow(glm::vec2(p.Left, p.Height));
	glm::vec2 right = MM.DesktopToWindow(glm::vec2(p.Right, p.Height));

	p.Left = left.x;
	p.Right = right.x;
	p.Height = left.y;

	// Add to the correct lists
	Platforms.push_back(p);
	WindowPlatforms[p.Hwnd].push_back(p);
}



BOOL CWindowsManager::EnumWindowCallback(HWND hWnd, LPARAM lparam)
{
	//if (GINPUT::GetKeyInputState('D') == InputState::INPUT_DOWN_FIRST)
	//{
	//	HELPER::PrintToOutput(CWindowsManager::GetInstance().GetWindowName(hWnd));
	//}

	LONG lStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);

	if (lStyle & WS_VISIBLE && lStyle & WS_EX_APPWINDOW && !IsIconic(hWnd) && hWnd != CScene::hWnd)
	{
		// If the window is cloaked it is not visible
		INT nCloaked;
		DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &nCloaked, sizeof(INT));
		IfThenStatement(nCloaked, return TRUE);

		// Remove any windows without a title
		int length = GetWindowTextLength(hWnd);
		IfThenStatement(length <= 0, return TRUE);

		// Remove all windows without a width or height 
		RECT r;
		DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(RECT));
		int w = r.right - r.left;
		int h = r.bottom - r.top;
		IfThenStatement(w <= 1 && h <= 1, return TRUE);

		// Get the windows title
		wchar_t* buffer = new wchar_t[length + 1];
		GetWindowText(hWnd, buffer, length + 1);
		std::wstring text = buffer;
		std::string windowTitle(text.begin(), text.end());
		delete[] buffer;

		// Special cases:
		if (windowTitle == "Program Manager" || windowTitle == "Cat")
		{
			return TRUE;
		}

		// Add windows to the visible windows list
		Instance->VisibleWindows.push_back(hWnd);
		Instance->VisibleWindowsBounds.insert({ hWnd, r });

		// Get windows state (fullscreen or not)
		//WINDOWPLACEMENT* placement = new WINDOWPLACEMENT();
		//placement->length = sizeof(WINDOWPLACEMENT);
		//GetWindowPlacement(hWnd, placement);

		//if (placement->showCmd != SW_SHOWMAXIMIZED)
		//{
		//	Instance->CollisionWindows.push_back(hWnd);
		//}

		//delete placement;

		//SetWindowPos(hWnd, NULL, 0, 0, 500, 500, 0);
		//SetWindowText(hWnd, L"Sweet Girl!");
	}

	return TRUE;
}
