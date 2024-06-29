#pragma once
#include "camera.h"

class CCamera2D : 
	public CCamera
{
public:
	CCamera2D(glm::vec2 camPos);
	virtual ~CCamera2D() {}

	virtual void Update() override;

protected:
	glm::vec2 CamPos;
};

