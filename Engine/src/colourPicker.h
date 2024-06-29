#pragma once

#include "object.h"



class CColourPicker :
	public CObject
{
public:
	CColourPicker(glm::vec2 location, float size);

	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;

	glm::vec3 GetRGB(bool& isRainbow);
	void SetRGB(glm::vec3 colour, bool isRainbow = false);
	void SetIsVisible(bool isVisible);
	bool GetIsVisible();

protected:
	void UpdatePickers();
	void UpdateFocus();
	void UpdateHue();
	void UpdateHue(float newHue);
	void UpdateSaturationValue();
	void UpdateSaturationValue(float newSaturation, float newValue);
	

	enum class PickerState
	{
		MovingHue,
		MovingSatVal,
		Waiting
	};

	glm::vec2 MouseLocation;

	bool IsVisible;
	bool IsRainbow;

	const float MaxRadiusHue = 0.975f;
	const float MinRadiusHue = 0.875f;

	const float MaxRadiusOutline = 1.00f;
	const float MinRadiusOutline = 0.85f;

	float Hue;
	float Saturation;
	float Value;

	PickerState State;

	CObject* HueObject;
	CObject* SaturationValue;

	CObject* HueOutline;
	CObject* SaturationValueOutline;

	CObject* HuePicker;
	CObject* SaturationValuePicker;
};

