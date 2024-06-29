#include "backgroundLayer.h"
#include "texture.h"
#include "graphics.h"
#include "camera.h"

CBackgroundLayer::CBackgroundLayer(CTexture* aTexture)
{
	BackgroundTexture = aTexture;
	LayerType = "CBackgroundLayer";
}

void CBackgroundLayer::Render()
{
	glm::mat4 pvm = CCamera::GetProjMatrix() * GFX::CalculateModelMatrix(glm::vec2(), glm::vec2(CCamera::GetScreenWidth(), CCamera::GetScreenHeight()));
	GFX::DrawImage(BackgroundTexture->GetID(), pvm);
}

void CBackgroundLayer::Update()
{
}
