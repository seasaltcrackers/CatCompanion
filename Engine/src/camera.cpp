#include "camera.h"

glm::mat4 CCamera::ViewMatrix;
glm::mat4 CCamera::ProjMatrix;
glm::mat4 CCamera::ProjViewMatrix;

unsigned int CCamera::ScreenWidth;
unsigned int CCamera::ScreenHeight;

/*
	Retrieve the current view matrix

	@return The mat4 current view matrix
*/
glm::mat4& CCamera::GetViewMatrix()
{
	return ViewMatrix;
}

/*
	Retrieve the current projection matrix

	@return The mat4 current projection matrix
*/
glm::mat4& CCamera::GetProjMatrix()
{
	return ProjMatrix;
}

glm::mat4& CCamera::GetProjViewMatrix()
{
	return ProjViewMatrix;
}

unsigned int CCamera::GetScreenWidth()
{
	return ScreenWidth;
}

unsigned int CCamera::GetScreenHeight()
{
	return ScreenHeight;
}

glm::vec2 CCamera::GetScreenSize()
{
	return glm::vec2(ScreenWidth, ScreenHeight);
}

glm::ivec2 CCamera::GetScreenSizeI()
{
	return glm::ivec2(ScreenWidth, ScreenHeight);
}

void CCamera::SetScreenSize(unsigned int screenWidth, unsigned int screenHeight)
{
	ScreenWidth = screenWidth;
	ScreenHeight = screenHeight;
}
