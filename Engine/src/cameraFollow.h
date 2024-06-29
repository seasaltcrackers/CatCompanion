#pragma once
#include "camera2D.h"

class CObject;

class CCameraFollow : 
	public CCamera2D
{
public:
	CCameraFollow(CObject* targetObject, glm::vec2 boundryTopLeft, glm::vec2 boundryBotRight);
	void Update() override;
	void SnapToTarget();

	glm::vec2 BoundryTopLeft;
	glm::vec2 BoundryBotRight;

private:
	CObject* TargetObject;
};

