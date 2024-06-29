#include "focusManager.h"

CFocusManager* CFocusManager::Instance;

CFocusManager& CFocusManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new CFocusManager();
		Instance->Update();
	}

	return *Instance;
}

void CFocusManager::DestroyInstance()
{
	delete Instance;
	Instance = nullptr;
}

CFocusManager::~CFocusManager()
{
}

void CFocusManager::Update()
{
	SetWindowState(NewFocus);
	NewFocus = false;
}

bool CFocusManager::GetIsFocused()
{
	return CurrentFocus;
}

void CFocusManager::SetFocused()
{
	NewFocus = true;
}

void CFocusManager::SetWindowState(bool isFocused)
{
	if (CurrentFocus != isFocused)
	{
		CurrentFocus = isFocused;
		//SetWindowLong(WindowHwnd, GWL_EXSTYLE, isFocused ? CurStyleFocused : CurStyleUnfocused);

		if (isFocused)
		{
			LONG cur_style = GetWindowLong(WindowHwnd, GWL_EXSTYLE);
			SetWindowLong(WindowHwnd, GWL_EXSTYLE, cur_style ^ WS_EX_TRANSPARENT ^ WS_EX_LAYERED);
		}
		else
		{
			LONG cur_style = GetWindowLong(WindowHwnd, GWL_EXSTYLE);
			SetWindowLong(WindowHwnd, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);
		}
	}
}

CFocusManager::CFocusManager() :
	WindowHwnd(CScene::hWnd)
{
	LONG curStyle = GetWindowLong(CScene::hWnd, GWL_EXSTYLE);
	CurStyleFocused = curStyle ^ WS_EX_TRANSPARENT ^ WS_EX_LAYERED;
	CurStyleUnfocused = curStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED;
}

CFocusManager::CFocusManager(CFocusManager const&) :
	WindowHwnd(CScene::hWnd)
{
	LONG curStyle = GetWindowLong(CScene::hWnd, GWL_EXSTYLE);
	CurStyleFocused = curStyle ^ WS_EX_TRANSPARENT ^ WS_EX_LAYERED;
	CurStyleUnfocused = curStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED;
}