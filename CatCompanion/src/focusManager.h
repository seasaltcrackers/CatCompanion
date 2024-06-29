#pragma once
#include <Windows.h>

#include "scene.h"

class CFocusManager
{
public:
	static CFocusManager& GetInstance();
	static void DestroyInstance();

	~CFocusManager();

	void Update();

	bool GetIsFocused();
	void SetFocused();
	
private:
	static CFocusManager* Instance;

	HWND& WindowHwnd;

	LONG CurStyleFocused;
	LONG CurStyleUnfocused;

	bool CurrentFocus = false;
	bool NewFocus = false;

	CFocusManager();
	CFocusManager(CFocusManager const&);

	void SetWindowState(bool isFocused);
};
