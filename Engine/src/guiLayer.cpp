#include <gtc\matrix_transform.hpp>

#include "guiLayer.h"
#include "camera.h"

CGUILayer::CGUILayer()
{
	ProjectionMatrix = glm::ortho(0.0f, (float)CCamera::GetScreenWidth(), (float)CCamera::GetScreenHeight(), 0.0f, -0.1f, 100.0f);
	LayerType = "CGUILayer";
}

void CGUILayer::Render()
{
	glm::mat4& pv = CCamera::GetProjViewMatrix();
	glm::mat4 old = pv;
	pv = ProjectionMatrix;

	CInstanceLayer::Render();

	pv = old;
}
