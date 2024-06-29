#pragma once
#include "camera3D.h"

class CCameraRotate3D :
	public CCamera3D
{
public:
	CCameraRotate3D(glm::vec3 center, float radius);
	virtual ~CCameraRotate3D() {}

	void Update() override;
	virtual void InputUpdate();

	void AddToRadius(float deltaRadius);
	void SetRadius(float newRadius);
	float GetRadius();

protected:
	const float Sensitivity = 10.0f;
	float Scale = 1.0f;

	bool IsSelected = false;
	glm::vec2 PreviousMousePosition;

	float TargetYaw;
	float TargetPitch;
	float TargetRadius;

	float Yaw;
	float Pitch;
	float Radius;
};