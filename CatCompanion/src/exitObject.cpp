#include "exitObject.h"
#include "input.h"
#include "graphics.h"
#include "monitorManager.h"
#include "timer.h"
#include "font.h"
#include "scene.h"

CExitObject::CExitObject() :
	CObject(nullptr, nullptr)
{
	IsHeld = false;
	HeldTime = 0.0f;
	Alpha = 0.0f;

	// Get the details of the main monitor
	CMonitorManager& MM = CMonitorManager::GetInstance();
	MonitorInfo detail = MM.GetMonitorDetail((int)MM.GetMonitors()[0]);
	
	// Set the location to be the top left of that monitor
	Location.y = detail.BoundryTB.top + Padding;
	Location.x = detail.BoundryTB.left + Padding;

	Font = CFont::GetFont("AcknowledgeTTSmall");
	TextBounds = Font->TextBounds(Text, 1.0f);
}

void CExitObject::Update()
{
	InputState escState = GINPUT::GetKeyInputState(27);

	if (!IsHeld)
	{
		// Check if the escape button has been pressed
		if (escState == InputState::INPUT_DOWN_FIRST ||
			escState == InputState::INPUT_DOWN)
		{
			// Start the escape sequence
			HoldStartTime = std::chrono::steady_clock::now();
			HeldTime = 0.0f;
			IsHeld = true;
		}
		else if (Alpha > 0.0f)
		{
			// Fade out
			Alpha -= TIMER::GetDeltaSeconds() * AlphaRate;
			Alpha = std::fmaxf(Alpha, 0.0f);
		}
	}
	else
	{
		// Check if the escape button has been released
		if (escState == InputState::INPUT_UP_FIRST ||
			escState == InputState::INPUT_UP)
		{
			// Revert back to normal
			IsHeld = false;
		}
		else
		{
			// Calculate how long it has been held for
			std::chrono::duration<float> tempTotalSec = std::chrono::steady_clock::now() - HoldStartTime;
			HeldTime = tempTotalSec.count();

			// Check if enough time has passed to quit the application
			if (HeldTime > TotalTimeRequired)
			{
				CScene::IsOpen = false;
				return;
			}
			else if (Alpha < 1.0f && HeldTime > FuzzyTime)
			{
				// Fade in
				Alpha += TIMER::GetDeltaSeconds() * AlphaRate;
				Alpha = std::fminf(Alpha, 1.0f);
			}
		}
	}

	CObject::Update();
}

void CExitObject::Draw(glm::mat4& pv)
{
	if (Alpha > 0.0f)
	{
		float fillWidth = Width * (HeldTime / TotalTimeRequired);

		// Draw the bar
		GFX::DrawRectangle(pv, glm::vec4(ColourLeft, Alpha), glm::vec2(Location), glm::vec2(fillWidth, Height));
		GFX::DrawRectangle(pv, glm::vec4(ColourRight, Alpha), glm::vec2(Location.x + fillWidth, Location.y), glm::vec2(Width - fillWidth, Height));
		GFX::DrawString(pv, Font, Text, glm::ivec2(Location) + glm::ivec2(Width * 0.5f, Height * 0.5f) - TextBounds / 2, glm::vec4(0.0f, 0.0f, 0.0f, Alpha));
	}
}
