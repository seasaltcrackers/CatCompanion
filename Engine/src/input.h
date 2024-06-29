#pragma once
#include <glm.hpp>
#include <vector>

enum class InputState
{
	INPUT_UP,
	INPUT_DOWN,
	INPUT_UP_FIRST,
	INPUT_DOWN_FIRST,
};

// Global Input (Works outside of the window)
namespace GINPUT
{
	// Use for applications without input events
	void UpdateGlobal();

	// Use for window applications
	void UpdateLocal();
	void KeyDown(unsigned char key);
	void KeyUp(unsigned char key);
	void SpecialKeyDown(int key);
	void SpecialKeyUp(int key);
	void MouseMove(int x, int y);
	void MouseClick(int button, int state);

	std::vector<unsigned int>& GetUpdatedKeys();
	InputState GetKeyInputState(unsigned int i);
	bool IsKeyDown(unsigned int i);
	bool IsKeyUp(unsigned int i);

	glm::vec2 GetMouseLocation();
	glm::vec2 GetOldMouseLocation();
	glm::vec2 GetDeltaMouseLocation();
	int GetDeltaScroll();

	InputState GetLMButtonState();
	InputState GetRMButtonState();
	InputState GetMMButtonState();
}