#include <algorithm>

#include "slider.h"
#include "graphics.h"
#include "input.h"
#include "helper.h"

CSlider::CSlider(float& value, float min, float max, glm::vec2 location, float width) :
	CObject(nullptr, nullptr), Value(value)
{
	Location = glm::vec3(location, 0.0f);
	
	Width = width;
	Percentage = (value - min) / (max - min);
	MinAmount = min;
	MaxAmount = max;
}

void CSlider::Update()
{
	if (GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST)
	{
		glm::vec2 loc = GINPUT::GetMouseLocation();
		if (loc.x > Location.x && loc.y > Location.y - Height * 0.5f &&
			loc.x < Location.x + Width && loc.y < Location.y + Height * 0.5f)
		{
			IsSelected = true;
		}
	}
	else if (GINPUT::GetLMButtonState() == InputState::INPUT_UP_FIRST)
	{
		IsSelected = false;
	}

	if (IsSelected)
	{
		glm::vec2 loc = GINPUT::GetMouseLocation();
		Percentage = std::clamp((loc.x - Location.x) / Width, 0.0f, 1.0f);
		Value = HELPER::Lerp(MinAmount, MaxAmount, Percentage);
	}
}

void CSlider::Draw(glm::mat4& pv)
{
	GFX::DrawLine(pv, { 1.0f, 1.0f, 1.0f }, { Location.x, Location.y }, { Location.x + Width, Location.y }, 4);																		// Base Line
	GFX::DrawLine(pv, { 1.0f, 1.0f, 1.0f }, { Location.x - 4, Location.y - Height * 0.5f }, { Location.x - 4, Location.y + Height * 0.5f }, 4);										// Left Line
	GFX::DrawLine(pv, { 1.0f, 1.0f, 1.0f }, { Location.x + Width + 4, Location.y - Height * 0.5f }, { Location.x + Width + 4, Location.y + Height * 0.5f }, 4);						// Right Line
	GFX::DrawLine(pv, { 1.0f, 1.0f, 1.0f }, { Location.x + Width * Percentage, Location.y - Height * 0.5f }, { Location.x + Width * Percentage, Location.y + Height * 0.5f }, 4);	// Value
}

void CSlider::SetValue(float newValue)
{
	newValue = std::clamp(newValue, MinAmount, MaxAmount);
	Percentage = (newValue - MinAmount) / (MaxAmount - MinAmount);
	Value = newValue;
}
