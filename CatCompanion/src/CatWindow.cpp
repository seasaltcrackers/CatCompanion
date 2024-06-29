//#define _WIN32_WINNT 0x0500

//#include <vld.h>

#include <glew.h>
#include <freeglut.h>

#include <OleCtl.h>
#include <windows.h>
#include <shellapi.h>
#include <strsafe.h>
#include <chrono>
#include <dwmapi.h>
#include <filesystem>
#pragma comment (lib, "dwmapi.lib")

#include <assert.h>
#include <tchar.h>

#include "CatWindow.h"
#include "program.h"
#include "helper.h"
#include "object.h"
#include "cat.h"
#include "input.h"
#include "scene.h"
#include "room.h"
#include "monitorManager.h"
#include "instanceLayer.h"
#include "mesh.h"
#include "configs.h"

#ifdef  assert
#define verify(expr) if(!expr) assert(0)
#else verify(expr) expr
#endif


const TCHAR szAppName[] = _T("Cat");
const TCHAR wcWndName[] = _T("Cat");

HDC hDC;
HGLRC m_hrc;

CScene* MainScene;

CConfigs* CGlobalVariables::ConfigSetting = nullptr;

BOOL initSC()
{
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);

	return 0;
}

void resizeSC(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL renderSC()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	MainScene->Render();
	glPopMatrix();
	glFlush();
	glFinish();

	return 0;
}

BOOL CreateHGLRC(HWND hWnd)
{
	PIXELFORMATDESCRIPTOR pfd = {
	  sizeof(PIXELFORMATDESCRIPTOR),
	  1,                                // Version Number
	  PFD_DRAW_TO_WINDOW |				// Format Must Support Window
	  PFD_SUPPORT_OPENGL |				// Format Must Support OpenGL
	  PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
	  PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
	  PFD_TYPE_RGBA,                    // Request An RGBA Format
	  32,                               // Select Our Color Depth
	  0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
	  8,                                // An Alpha Buffer
	  0,                                // Shift Bit Ignored
	  0,                                // No Accumulation Buffer
	  0, 0, 0, 0,                       // Accumulation Bits Ignored
	  24,                               // 16Bit Z-Buffer (Depth Buffer)
	  8,                                // Some Stencil Buffer
	  0,                                // No Auxiliary Buffer
	  PFD_MAIN_PLANE,                   // Main Drawing Layer
	  0,                                // Reserved
	  0, 0, 0                           // Layer Masks Ignored
	};

	HDC hdc = GetDC(hWnd);
	int PixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (PixelFormat == 0) {
		assert(0);
		return FALSE;
	}

	BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
	if (bResult == FALSE) {
		assert(0);
		return FALSE;
	}

	m_hrc = wglCreateContext(hdc);
	if (!m_hrc) {
		assert(0);
		return FALSE;
	}

	ReleaseDC(hWnd, hdc);

	return TRUE;
}

GLuint HiconToTexture(HICON hIcon);

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_CREATE:
			//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
			break;
		case WM_DESTROY:
		{
			if (m_hrc)
			{
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(m_hrc);
			}
			
			PostQuitMessage(0);
			break;
		}
		//case WM_DROPFILES:
		//{	
		//	HDROP hDropInfo = (HDROP)wParam;
		//	char sItem[MAX_PATH];
		//	DragQueryFile(hDropInfo, 0, (LPWSTR)sItem, sizeof(sItem));

		//	// Get the filepath
		//	// -----------------------------------
		//	//int i = 0;
		//	//char current = sItem[i];
		//	//std::string filePath = "";

		//	//do 
		//	//{
		//	//	if (current != 0) 
		//	//	{
		//	//		filePath += current;
		//	//	}

		//	//	++i;
		//	//	current = sItem[i];

		//	//} while (current != -52 && i <= MAX_PATH);
		//	// -----------------------------------

		//	// Get the icon and create a texture from it
		//	WORD* word = new WORD();
		//	HICON icon = ExtractAssociatedIcon(NULL, (LPWSTR)sItem, word);

		//	MainScene->OnFileDropped(icon);

		//	delete word;
		//	word = nullptr;
		//	break;
		//}
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode) 
{
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm = LoadIcon(hThisInst, L"small.ico");
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hThisInst;
	wc.hIcon = LoadIcon(hThisInst, L"CatCompanion.ico");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
	wc.lpszClassName = szAppName;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, _T("RegisterClassEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}


	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	CConfigSettings settings;
	settings.SetDefaultFilename("Configs");
	settings.SetFileLocation("");

	settings.AddDefaultStringVariable("monitors", "auto");
	settings.AddDefaultStringVariable("f3-debug", "disabled");


	if (std::filesystem::exists("Configs.txt"))
	{
		CGlobalVariables::ConfigSetting = new CConfigs(settings, "Configs");
	}
	else
	{
		CGlobalVariables::ConfigSetting = new CConfigs(settings);
	}

	CMonitorManager& MM = CMonitorManager::GetInstance();
	RECT winCombined = MM.GetCombinedRect();

	float margin = 1.0f;
	//winCombined.left -= margin;
	//winCombined.right += margin;
	//winCombined.top -= margin;
	//winCombined.bottom += margin;

	int winWidth = winCombined.right - winCombined.left;
	int winHeight = winCombined.bottom - winCombined.top;
		
	HWND hWnd = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, szAppName, wcWndName,
							   WS_VISIBLE | WS_POPUP, winCombined.left - margin, winCombined.top - margin, winWidth + margin * 2.0f, winHeight + margin * 2.0f,
							   NULL, NULL, hThisInst, NULL);

	LONG cur_style = GetWindowLong(hWnd, GWL_EXSTYLE);
	SetWindowLong(hWnd, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);

	if (!hWnd) {
		MessageBox(NULL, _T("CreateWindowEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	DWM_BLURBEHIND bb = { };
	HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
	bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	bb.hRgnBlur = hRgn;
	bb.fEnable = TRUE;
	DwmEnableBlurBehindWindow(hWnd, &bb);

	CreateHGLRC(hWnd);

	HDC hdc = GetDC(hWnd);
	wglMakeCurrent(hdc, m_hrc);

	glewInit();
	initSC();

	MainScene = new CScene(hWnd, winWidth, winHeight);
	MainScene->Initialise();

	//resizeSC(margin, margin, winWidth + margin * 2.0f, winHeight + margin * 2.0f);
	resizeSC(margin, margin, winWidth, winHeight);
	ReleaseDC(hWnd, hdc);

	MSG msg;

	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point thisTime;
	int TargetFPS = (int)roundf(1000.0f / 60.0f);

	CScene::IsOpen = true;

	//while (GetAsyncKeyState(VK_ESCAPE) == 0) {
	while (CScene::IsOpen) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			MainScene->Update();

			HDC hdc = GetDC(hWnd);
			wglMakeCurrent(hdc, m_hrc);

			renderSC();

			SwapBuffers(hdc);
			ReleaseDC(hWnd, hdc);


			//thisTime = std::chrono::high_resolution_clock::now();
			//std::chrono::duration<float, std::milli> delta = thisTime - lastTime;
			//float deltaTime = std::max(delta.count(), 0.0f);
			//lastTime = thisTime;
			//Sleep(std::max(TargetFPS - deltaTime, 0.0f));
		}
	}


	MainScene->CleanUp();

	delete MainScene;
	MainScene = nullptr;

	delete CGlobalVariables::ConfigSetting;
	CGlobalVariables::ConfigSetting = nullptr;

	return (FALSE);
}
