#include <gtc/matrix_transform.hpp>

#include "camera2D.h"

CCamera2D::CCamera2D(glm::vec2 camPos)
{
	CamPos = camPos;
}

void CCamera2D::Update()
{
	// View Matrix
	ViewMatrix = glm::lookAt(glm::vec3(glm::round(CamPos), 1.0f), glm::vec3(glm::round(CamPos), 1.0f) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Projection Matrix
	ProjMatrix = glm::ortho(0.0f, (float)ScreenWidth, (float)ScreenHeight, 0.0f, -0.1f, 100.0f);

	// ProjView Matrix
	ProjViewMatrix = ProjMatrix * ViewMatrix;
}
