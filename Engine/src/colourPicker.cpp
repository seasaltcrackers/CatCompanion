#include "colourPicker.h"
#include "helper.h"
#include "input.h"
#include "program.h"
#include "mesh.h"
#include "graphics.h"
#include "timer.h"

#include <algorithm>

CColourPicker::CColourPicker(glm::vec2 location, float size) :
	CObject()
{
	State = PickerState::Waiting;
	Location = glm::vec3(location, 0.0f);
	Scale *= size;

	Hue = 0.0f;
	Saturation = 1.0f;
	Value = 1.0f;

	// Hue ring
	// -----------------------------------------------------------------------------------
	HueObject = new CObject(CProgram::GetProgram("ColourHue"), CMesh::GetMesh("3D Square"));
	HueObject->UpdateUniformFloat("maxRadius", MaxRadiusHue);
	HueObject->UpdateUniformFloat("minRadius", MinRadiusHue);
	HueObject->Location = Location;
	HueObject->Scale = Scale;
	// -----------------------------------------------------------------------------------

	// Hue outline
	// -----------------------------------------------------------------------------------
	HueOutline = new CObject(CProgram::GetProgram("Ring"), CMesh::GetMesh("3D Square"));
	HueOutline->UpdateUniformfVec4("ringColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	HueOutline->UpdateUniformFloat("maxRadius", MaxRadiusOutline);
	HueOutline->UpdateUniformFloat("minRadius", MinRadiusOutline);
	HueOutline->Location = Location;
	HueOutline->Scale = Scale;
	// -----------------------------------------------------------------------------------

	// Saturation/Value square
	// -----------------------------------------------------------------------------------
	SaturationValue = new CObject(CProgram::GetProgram("ColourSatVal"), CMesh::GetMesh("3D Square"));
	SaturationValue->UpdateUniformFloatR("hue", Hue);
	SaturationValue->Location = Location;
	SaturationValue->Scale *= Scale * MinRadiusOutline;
	SaturationValue->Scale /= sqrtf(2.0f);
	// -----------------------------------------------------------------------------------

	// Saturation/Value outline
	// -----------------------------------------------------------------------------------
	SaturationValueOutline = new CObject(CProgram::GetProgram("Colour"), CMesh::GetMesh("3D Square"));
	SaturationValueOutline->UpdateUniformfVec4("objColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SaturationValueOutline->Location = Location;
	SaturationValueOutline->Scale *= Scale * MinRadiusHue;
	SaturationValueOutline->Scale /= sqrtf(2.0f);
	SaturationValueOutline->Scale *= 1.01f;
	// -----------------------------------------------------------------------------------

	// Picker for hue
	// -----------------------------------------------------------------------------------
	HuePicker = new CObject(CProgram::GetProgram("Ring"), CMesh::GetMesh("3D Square"));
	HuePicker->UpdateUniformfVec4("ringColour", glm::vec4(1.0f, 1.0f, 1.0f, 0.9f));
	HuePicker->UpdateUniformFloat("maxRadius", 1.0f);
	HuePicker->UpdateUniformFloat("minRadius", 0.6f);
	HuePicker->Scale *= (Scale.x * (MaxRadiusHue - MinRadiusHue) * 0.8f);
	HuePicker->Location = Location + glm::vec3(Scale.x * (MaxRadiusHue + MinRadiusHue) * 0.25f, 0.0f, 0.0f);
	// -----------------------------------------------------------------------------------

	// Picker for saturation and value
	// -----------------------------------------------------------------------------------
	SaturationValuePicker = new CObject(CProgram::GetProgram("Ring"), CMesh::GetMesh("3D Square"));
	SaturationValuePicker->UpdateUniformfVec4("ringColour", glm::vec4(1.0f, 1.0f, 1.0f, 0.9f));
	SaturationValuePicker->UpdateUniformFloat("maxRadius", 1.0f);
	SaturationValuePicker->UpdateUniformFloat("minRadius", 0.6f);
	SaturationValuePicker->Scale *= HuePicker->Scale;
	SaturationValuePicker->Location = Location - 0.5f * glm::vec3(SaturationValue->Scale.x, SaturationValue->Scale.y, 0.0f);
	// -----------------------------------------------------------------------------------
}

void CColourPicker::Update()
{
	IfThenReturn(!IsVisible,)

	// Follow the colour picker
	HueObject->Location = Location;
	SaturationValue->Location = Location;

	HueOutline->Location = Location;
	SaturationValueOutline->Location = Location;

	UpdatePickers();

	// Update Objects
	HueObject->Update();
	SaturationValue->Update();

	HuePicker->Update();
	SaturationValuePicker->Update();

	HueOutline->Update();
	SaturationValueOutline->Update();
}

void CColourPicker::Draw(glm::mat4& pv)
{
	IfThenReturn(!IsVisible, )

	// Draw outlines
	HueOutline->Draw(pv);
	SaturationValueOutline->Draw(pv);

	// Draw base colours
	HueObject->Draw(pv);
	SaturationValue->Draw(pv);

	// Draw pickers
	HuePicker->Draw(pv);
	SaturationValuePicker->Draw(pv);

	// Draw output colour
	if (State != PickerState::Waiting)
	{
		GFX::DrawRectangle(pv, GetRGB(IsRainbow), glm::vec2(Location) + glm::vec2(Scale.x * 0.5f + 40.0f, 0.0f) - glm::vec2(25.0f, 25.0f), glm::vec2(50.0f, 50.0f));
	}
}

glm::vec3 CColourPicker::GetRGB(bool& isRainbow)
{
	isRainbow = IsRainbow;
	return HELPER::HSVtoRGB((int)(Hue * 360.0f), (double)Saturation, (double)Value) / 255.0f;
}

void CColourPicker::SetRGB(glm::vec3 colour, bool isRainbow)
{
	IsRainbow = isRainbow;
	glm::vec3 hsv = HELPER::RGBtoHSV(colour.x * 255.0f, colour.y * 255.0f, colour.z * 255.0f);

	UpdateHue(hsv.x / 360.0f);
	UpdateSaturationValue(hsv.y, hsv.z);
}

void CColourPicker::SetIsVisible(bool isVisible)
{
	IsVisible = isVisible;
}

bool CColourPicker::GetIsVisible()
{
	return IsVisible;
}

void CColourPicker::UpdatePickers()
{
	if (IsRainbow)
	{
		UpdateHue(std::fmodf(Hue + TIMER::GetDeltaSeconds() * 0.5f, 1.0f));
	}

	if (State == PickerState::Waiting)
	{
		if (GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST)
		{
			glm::vec2 delta = MouseLocation - glm::vec2(Location);

			if (std::abs(delta.x) <= SaturationValue->Scale.x * 0.5f &&
				std::abs(delta.y) <= SaturationValue->Scale.y * 0.5f)
			{
				State = PickerState::MovingSatVal;
			}
			else
			{
				float length = glm::length(delta);
				length /= HueObject->Scale.x * 0.5f;

				if (length <= MaxRadiusHue && length >= MinRadiusHue)
				{
					State = PickerState::MovingHue;
				}
			}
		}
	}

	if (State == PickerState::MovingHue)
	{
		glm::vec2 delta = MouseLocation - glm::vec2(Location);
		glm::vec2 newLoc = HELPER::SetMag(delta, Scale.x * (MaxRadiusHue + MinRadiusHue) * 0.25f);

		if (std::abs(delta.x) < 1.0f && std::abs(delta.y) < 1.0f)
		{
			IsRainbow = true;
		}
		else
		{
			IsRainbow = false;

			newLoc = glm::vec2(Location) + newLoc;
			HuePicker->Location = glm::vec3(newLoc, 0.0f);

			UpdateHue();
		}


		if (GINPUT::GetLMButtonState() == InputState::INPUT_UP ||
			GINPUT::GetLMButtonState() == InputState::INPUT_UP_FIRST)
		{
			State = PickerState::Waiting;
		}
	}
	else if (State == PickerState::MovingSatVal)
	{
		glm::vec2 delta = MouseLocation - glm::vec2(Location);
		glm::vec2 halfScale = glm::vec2(SaturationValue->Scale) * 0.5f;

		glm::vec2 newLoc = glm::clamp(delta, -halfScale, halfScale);
		newLoc = glm::vec2(Location) + newLoc;
		SaturationValuePicker->Location = glm::vec3(newLoc, 0.0f);

		UpdateSaturationValue();

		if (GINPUT::GetLMButtonState() == InputState::INPUT_UP ||
			GINPUT::GetLMButtonState() == InputState::INPUT_UP_FIRST)
		{
			State = PickerState::Waiting;
		}
	}

	MouseLocation = GINPUT::GetMouseLocation();
}

void CColourPicker::UpdateHue()
{
	glm::vec3 hueDeltaLoc = Location - HuePicker->Location;
	Hue = std::atan2f(hueDeltaLoc.y, hueDeltaLoc.x);
	Hue = std::fmodf(Hue + PI * 2.0f, PI * 2.0f);
	Hue /= (PI * 2.0f);
}

void CColourPicker::UpdateHue(float newValue)
{
	Hue = newValue;

	HuePicker->Location = glm::vec3(HELPER::LengthDirDeg(Scale.x * (MaxRadiusHue + MinRadiusHue) * 0.25f, -newValue * 360.0f), 0.0f);
	HuePicker->Location.x *= -1;
	HuePicker->Location += Location;
}

void CColourPicker::UpdateSaturationValue()
{
	glm::vec3 satValDeltaLoc = Location - SaturationValuePicker->Location;
	satValDeltaLoc += SaturationValue->Scale * 0.5f;
	satValDeltaLoc /= SaturationValue->Scale;

	Saturation = std::clamp(satValDeltaLoc.x, 0.0f, 1.0f);
	Value = std::clamp(satValDeltaLoc.y, 0.0f, 1.0f);
}

void CColourPicker::UpdateSaturationValue(float newSaturation, float newValue)
{
	Saturation = newSaturation;
	Value = newValue;

	SaturationValuePicker->Location = Location + (SaturationValue->Scale * glm::vec3(1.0f - newSaturation, 1.0f - newValue, 1.0f) - SaturationValue->Scale * 0.5f);
	SaturationValuePicker->Location.z = 0.0f;
}
