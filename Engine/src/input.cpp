#include <Windows.h>
#include <iostream>
#include <string>
#include <glew.h>
#include <glut.h>
#include <vector>

#include "input.h"
#include "helper.h"

static InputState KeyStates[256] = { };
static InputState MouseState[3] = { };
static std::vector<unsigned int> KeyStatesToBeUpdated;
static int DeltaScroll = 0;

static glm::vec2 MouseLocation = glm::vec2();
static glm::vec2 MouseLocationOld = glm::vec2();

/*
	Updates the global mouse location and key states (Should be called at the start of update)
*/
void GINPUT::UpdateGlobal()
{
	KeyStatesToBeUpdated.clear();

	// Update Mouse Location
	POINT p;
	GetCursorPos(&p);
	
	MouseLocationOld = MouseLocation;
	MouseLocation = glm::vec2(p.x, p.y);

	// Update Key States
	BYTE keyStatesNew[256] = { };
	GetKeyState(0);
	if (GetKeyboardState(keyStatesNew))
	{
		for (int i = 0; i < 256; ++i)
		{
			// Key Down
			if (KeyStates[i] == InputState::INPUT_UP && keyStatesNew[i] >= 128)
			{
				KeyStates[i] = InputState::INPUT_DOWN_FIRST;
				KeyStatesToBeUpdated.push_back(i);
			}
			// Key Up
			else if (KeyStates[i] == InputState::INPUT_DOWN && keyStatesNew[i] <= 1)
			{
				KeyStates[i] = InputState::INPUT_UP_FIRST;
				KeyStatesToBeUpdated.push_back(i);
			}
			else if (KeyStates[i] == InputState::INPUT_DOWN_FIRST)
			{
				KeyStates[i] = InputState::INPUT_DOWN;
			}
			else if (KeyStates[i] == InputState::INPUT_UP_FIRST)
			{
				KeyStates[i] = InputState::INPUT_UP;
			}
		}
	}

	MouseState[0] = KeyStates[VK_LBUTTON];
	MouseState[1] = KeyStates[VK_MBUTTON];
	MouseState[2] = KeyStates[VK_RBUTTON];
}

/*
	Updates the old mouse location and switches input states from their first state to their normal state (Should be called at the end of update)
*/
void GINPUT::UpdateLocal()
{
	MouseLocationOld = MouseLocation;
	DeltaScroll = 0;

	for (int i = 0; i < 3; ++i)
	{
		IfThenStatement(MouseState[i] == InputState::INPUT_DOWN_FIRST, MouseState[i] = InputState::INPUT_DOWN);
		IfThenStatement(MouseState[i] == InputState::INPUT_UP_FIRST, MouseState[i] = InputState::INPUT_UP);
	}

	for (int i = 0; i < (int)KeyStatesToBeUpdated.size(); ++i)
	{
		IfThenStatement(KeyStates[KeyStatesToBeUpdated[i]] == InputState::INPUT_DOWN_FIRST, KeyStates[KeyStatesToBeUpdated[i]] = InputState::INPUT_DOWN);
		IfThenStatement(KeyStates[KeyStatesToBeUpdated[i]] == InputState::INPUT_UP_FIRST, KeyStates[KeyStatesToBeUpdated[i]] = InputState::INPUT_UP);
	}

	KeyStatesToBeUpdated.clear();
}

/*
	Should be called whenever a key is pressed

	@param key The character that has been released (e.g. 'a')
*/
void GINPUT::KeyDown(unsigned char key)
{
	if (KeyStates[key] == InputState::INPUT_UP)
	{
		KeyStates[key] = InputState::INPUT_DOWN_FIRST;
		KeyStatesToBeUpdated.push_back(key);
	}
}

/*
	Should be called whenever a key is released

	@param key The character that has been released (e.g. 'a')
*/
void GINPUT::KeyUp(unsigned char key)
{
	if (KeyStates[key] == InputState::INPUT_DOWN)
	{
		KeyStates[key] = InputState::INPUT_UP_FIRST;
		KeyStatesToBeUpdated.push_back(key);
	}

	if (key >= 'A' && key <= 'Z' && KeyStates[key + 32] == InputState::INPUT_DOWN)
	{
		KeyStates[key + 32] = InputState::INPUT_UP_FIRST;
		KeyStatesToBeUpdated.push_back(key + 32);
	}
	else if (key >= 'a' && key <= 'z' && KeyStates[key - 32] == InputState::INPUT_DOWN)
	{
		char test = key - 32;
		KeyStates[key - 32] = InputState::INPUT_UP_FIRST;
		KeyStatesToBeUpdated.push_back(key - 32);
	}
}

void GINPUT::SpecialKeyDown(int key)
{
	switch (key)
	{
		case 100:
			KeyDown(VK_LEFT);
			break;
		case 101:
			KeyDown(VK_UP);
			break;
		case 102:
			KeyDown(VK_RIGHT);
			break;
		case 103:
			KeyDown(VK_DOWN);
			break;
	}
}

void GINPUT::SpecialKeyUp(int key)
{
	switch (key)
	{
		case 100:
			KeyUp(VK_LEFT);
			break;
		case 101:
			KeyUp(VK_UP);
			break;
		case 102:
			KeyUp(VK_RIGHT);
			break;
		case 103:
			KeyUp(VK_DOWN);
			break;
	}
}

/*
	Should be called with the new mouse location everytime the mouse moves

	@param x The new x location of the cursor
	@param y The new y location of the cursor
*/
void GINPUT::MouseMove(int x, int y)
{
	MouseLocationOld = MouseLocation;
	MouseLocation = glm::vec2(x, y);
}

/*
	Updates the mouse state

	@param button The button pressed
	@param state The state of the button
*/
void GINPUT::MouseClick(int button, int state)
{
	IfThenReturn(button >= 5, );

	if (button <= 2)
	{
		InputState newState = (state == GLUT_DOWN ? InputState::INPUT_DOWN : InputState::INPUT_UP);
		if (newState == InputState::INPUT_DOWN)
		{
			MouseState[button] = (MouseState[button] == InputState::INPUT_UP) ? InputState::INPUT_DOWN_FIRST : InputState::INPUT_DOWN;
		}
		else
		{
			MouseState[button] = (MouseState[button] == InputState::INPUT_DOWN) ? InputState::INPUT_UP_FIRST : InputState::INPUT_UP;
		}
	}
	else
	{
		DeltaScroll -= (button == 3 ? 1 : -1);
	}
}

std::vector<unsigned int>& GINPUT::GetUpdatedKeys()
{
	return KeyStatesToBeUpdated;
}

/*
	Checks a keys state

	@param i The ID of the key to be checked (e.g 'H', VK_xxxxxx)
	@return Returns either INPUT_UP, INPUT_DOWN, INPUT_UP_FIRST or INPUT_DOWN_FIRST
*/
InputState GINPUT::GetKeyInputState(unsigned int i)
{
	return KeyStates[i];
}

/*
	Checks if a given key is down (no matter if it is the first or just a normal key down)

	@param i The int of the key (e.g. 'a')
	@return Returns true if the key is down
*/
bool GINPUT::IsKeyDown(unsigned int i)
{
	return KeyStates[i] == InputState::INPUT_DOWN || KeyStates[i] == InputState::INPUT_DOWN_FIRST;
}

/*
	Checks if a given key is up (no matter if it is the first or just a normal key up)

	@param i The int of the key (e.g. 'a')
	@return Returns true if the key is up
*/
bool GINPUT::IsKeyUp(unsigned int i)
{
	return KeyStates[i] == InputState::INPUT_UP || KeyStates[i] == InputState::INPUT_UP_FIRST;
}

/*
	Returns the last mouse location calculated

	@return Returns a vec2 of the mouse position
*/
glm::vec2 GINPUT::GetMouseLocation()
{
	return MouseLocation;
}

/*
	Returns the last old mouse location calculated

	@return Returns a vec2 of the old mouse position
*/
glm::vec2 GINPUT::GetOldMouseLocation()
{
	return MouseLocationOld;
}

/*
	Returns delta location between the new and old mouse location

	@return Returns a vec2 of the delta mouse position
*/
glm::vec2 GINPUT::GetDeltaMouseLocation()
{
	return MouseLocation - MouseLocationOld;
}

int GINPUT::GetDeltaScroll()
{
	return DeltaScroll;
}

InputState GINPUT::GetLMButtonState()
{
	return MouseState[0];
}

InputState GINPUT::GetRMButtonState()
{
	return MouseState[2];
}

InputState GINPUT::GetMMButtonState()
{
	return MouseState[1];
}
