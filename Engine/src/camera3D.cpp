#include <gtc\matrix_transform.hpp>

#include "input.h"
#include "camera3D.h"
#include "object.h"

CCamera3D::CCamera3D(glm::vec3 camPos, glm::vec3 camLookPos, glm::vec3 camUpDir)
{
	CamPos = camPos;
	CamLookPos = camLookPos;
	CamUpDir = camUpDir;
	Ray = nullptr;
}

void CCamera3D::Update()
{
	// View Matrix
	ViewMatrix = glm::lookAt(CamPos, CamLookPos, CamUpDir);

	// Projection Matrix
	ProjMatrix = glm::perspective(45.0f, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 10000.0f);

	// ProjView Matrix
	ProjViewMatrix = ProjMatrix * ViewMatrix;

	// Reset the ray
	if (Ray != nullptr)
	{
		delete Ray;
		Ray = nullptr;
	}
}


glm::vec3 CCamera3D::GetCamPos()
{
	return CamPos;
}
glm::vec3 CCamera3D::GetCamLookPos()
{
	return CamLookPos;
}

glm::vec3 CCamera3D::GetCamUpDir()
{
	return CamUpDir;
}

glm::vec3 CCamera3D::GetRayFromMouse()
{
	if (Ray == nullptr)
	{
		// Convert to normalised screen coordinates
		glm::vec2 mousePos = GINPUT::GetMouseLocation();
		mousePos.x = (mousePos.x * 2.0f) / (float)CCamera::GetScreenWidth() - 1.0f;
		mousePos.y = 1.0f - (mousePos.y * 2.0f) / (float)CCamera::GetScreenHeight();

		// Screen pos to projection space
		glm::vec4 clipCoords = glm::vec4(mousePos, -1.0f, 1.0f);

		// Projection space to eye space
		glm::mat4 invProjMat = glm::inverse(CCamera::GetProjMatrix());
		glm::vec4 eyeCoords = invProjMat * clipCoords;
		eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

		// Eye space to world space
		glm::mat4 invViewMat = glm::inverse(CCamera::GetViewMatrix());
		glm::vec4 rayWorld = invViewMat * eyeCoords;
		Ray = new glm::vec3(glm::normalize(glm::vec3(rayWorld)));
	}

	return *Ray;
}

void CCamera3D::AssignCameraPosition(CObject* anObj, std::string uniformName)
{
	anObj->UpdateUniformfVec3R(uniformName, CamPos);
}
