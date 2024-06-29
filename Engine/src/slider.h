#pragma once
#include "object.h"



class CSlider :
	public CObject
{
public:
	CSlider(float& value, float min, float max, glm::vec2 location, float width);
	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjMatrix()) override;

	void SetValue(float newValue);

private:
	const float Height = 10.0f;
	float Width;

	bool IsSelected;

	float& Value;
	float Percentage;
	float MinAmount;
	float MaxAmount;

};

