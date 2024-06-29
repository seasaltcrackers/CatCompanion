#include <iostream>
#include <algorithm>

#include "cameraRotate3D.h"
#include "timer.h"
#include "helper.h"
#include "object.h"
#include "input.h"

CCameraRotate3D::CCameraRotate3D(glm::vec3 center, float radius) :
	CCamera3D(glm::vec3(), center, glm::vec3(0.0f, 1.0f, 0.0f))
{
	CamLookPos = center;

	Radius = radius;
	Yaw = 0.0f;
	Pitch = 0.0f;

	TargetRadius = Radius;
	TargetYaw = Yaw;
	TargetPitch = Pitch;
}

void CCameraRotate3D::Update()
{
	// Update IsSelected based off of user input
	InputUpdate();

	if (IsSelected)
	{
		// Get current cursor location
		POINT tempCursorLoc = { 0, 0 };
		GetCursorPos(&tempCursorLoc);
		glm::vec2 mouseLocBefore = { tempCursorLoc.x, tempCursorLoc.y };

		// Move mouse to the middle of the screen
		glm::vec2 middle = CCamera::GetScreenSize() * 0.5f;
		glutWarpPointer((int)middle.x, (int)middle.y);

		// Get the new cursor location
		GetCursorPos(&tempCursorLoc);
		glm::vec2 mouseLocAfter = { tempCursorLoc.x, tempCursorLoc.y };

		// calculate the delta cursor location
		glm::vec2 delta = mouseLocAfter - mouseLocBefore;

		// Change Yaw and Pitch based off this delta change
		TargetYaw -= delta.x * 0.01f;
		TargetPitch -= delta.y * 0.01f;

		// Clamp (or this will glitch out)
		TargetPitch = std::clamp(TargetPitch, -(float)(PI) * 0.49f, (float)(PI) * 0.49f);
	}

	// Lerp current position/rotation to taget position/rotation for smoother camera flow
	float i = std::clamp(Sensitivity * TIMER::GetDeltaSeconds(), 0.0f, 1.0f);

	Yaw = HELPER::Lerp(Yaw, TargetYaw, i);
	Pitch = HELPER::Lerp(Pitch, TargetPitch, i);
	Radius = HELPER::Lerp(Radius, TargetRadius, i);

	CamPos.y = Radius * sin(Pitch);
	CamPos.x = cos(Pitch) * HELPER::LengthDirRadX(Radius, Yaw);
	CamPos.z = cos(Pitch) * HELPER::LengthDirRadY(Radius, Yaw);

	CamPos += CamLookPos;

	CCamera3D::Update();
}

void CCameraRotate3D::InputUpdate()
{
	// Click and hold to move:

	// get curremt state
	InputState state = GINPUT::GetMMButtonState();
	if (state == InputState::INPUT_DOWN_FIRST)
	{
		// Hide cursor
		IsSelected = true;
		glutSetCursor(GLUT_CURSOR_NONE);

		// Remember old position
		PreviousMousePosition = GINPUT::GetMouseLocation();

		// Send cursor to the center
		glm::vec2 middle = CCamera::GetScreenSize() * 0.5f;
		glutWarpPointer((int)middle.x, (int)middle.y);
	}
	else if (state == InputState::INPUT_UP_FIRST)
	{
		// Unhide cursor and set the cursor back to it's old location
		IsSelected = false;
		glutSetCursor(GLUT_CURSOR_INHERIT);
		glutWarpPointer((int)PreviousMousePosition.x, (int)PreviousMousePosition.y);
	}

	// Scrolling zooms in and out the camera
	TargetRadius += GINPUT::GetDeltaScroll() * 0.5f * Scale;
	TargetRadius = std::clamp(TargetRadius, 0.1f, 1000.0f);
}

void CCameraRotate3D::AddToRadius(float deltaRadius)
{
	TargetRadius = std::clamp(TargetRadius + deltaRadius, 0.1f, 1000.0f);
}

void CCameraRotate3D::SetRadius(float newRadius)
{
	TargetRadius = std::clamp(newRadius, 0.1f, 1000.0f);
}

float CCameraRotate3D::GetRadius()
{
	return TargetRadius;
}
