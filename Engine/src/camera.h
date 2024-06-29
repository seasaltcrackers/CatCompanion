#pragma once
#include <glm.hpp>


class CCamera
{
public:
	virtual ~CCamera() {}

	virtual void Update() = 0;

	static glm::mat4& GetViewMatrix();
	static glm::mat4& GetProjMatrix();
	static glm::mat4& GetProjViewMatrix();
	
	static unsigned int GetScreenWidth();
	static unsigned int GetScreenHeight();
	static glm::vec2  GetScreenSize();
	static glm::ivec2 GetScreenSizeI();

	static void SetScreenSize(unsigned int screenWidth, unsigned int screenHeight);

protected:
	static glm::mat4 ViewMatrix;
	static glm::mat4 ProjMatrix;
	static glm::mat4 ProjViewMatrix;

	static unsigned int ScreenWidth;
	static unsigned int ScreenHeight;
};


//namespace CAMERA
//{
//	void Initialise(unsigned int screenWidth, unsigned int screenHeight);
//
//	void Update3D();
//	void Update2D();
//
//	glm::mat4& GetViewMatrix();
//	glm::mat4& GetProjMatrix();
//	glm::mat4& GetProjViewMatrix();
//
//	unsigned int GetScreenWidth();
//	unsigned int GetScreenHeight();
//}