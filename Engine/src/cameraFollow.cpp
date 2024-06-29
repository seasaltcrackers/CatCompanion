#include "cameraFollow.h"
#include "object.h"
#include "timer.h"

CCameraFollow::CCameraFollow(CObject* targetObject, glm::vec2 boundryTopLeft, glm::vec2 boundryBotRight) :
	CCamera2D(glm::vec2(targetObject->Location) - glm::vec2(ScreenWidth, ScreenHeight) * 0.5f)
{
	TargetObject = targetObject;

	BoundryTopLeft = boundryTopLeft;
	BoundryBotRight = boundryBotRight - glm::vec2(ScreenWidth, ScreenHeight);
}

void CCameraFollow::Update()
{
	glm::vec2 deltaCamPos = (glm::vec2(TargetObject->Location) - glm::vec2(ScreenWidth, ScreenHeight) * 0.5f) - CamPos;
	CamPos = CamPos + deltaCamPos * std::min(TIMER::GetDeltaSeconds() * 2.0f, 1.0f);
	CamPos = glm::clamp(CamPos, BoundryTopLeft, BoundryBotRight);
	CCamera2D::Update();
}

void CCameraFollow::SnapToTarget()
{
	CamPos = glm::vec2(TargetObject->Location) - glm::vec2(ScreenWidth, ScreenHeight) * 0.5f;
}
