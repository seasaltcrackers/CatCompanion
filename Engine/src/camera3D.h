#pragma once
#include <string>

#include "camera.h"

class CObject;

class CCamera3D :
	public CCamera
{
public:
	CCamera3D(glm::vec3 camPos, glm::vec3 camLookPos, glm::vec3 camUpDir);
	virtual ~CCamera3D() {}

	virtual void Update() override;

	glm::vec3 GetCamPos();
	glm::vec3 GetCamLookPos();
	glm::vec3 GetCamUpDir();
	glm::vec3 GetRayFromMouse();

	void AssignCameraPosition(CObject* anObj, std::string uniformName);

protected:
	glm::vec3 CamPos;
	glm::vec3 CamLookPos;
	glm::vec3 CamUpDir;

	glm::vec3* Ray;
};

