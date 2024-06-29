#pragma once
#include "object.h"

class CFont;

class CLabel :
	public CObject
{
public:
	CLabel(CFont* font, std::string text, glm::vec2 location, glm::ivec3 colour = { 255.0f, 255.0f, 255.0f });
	~CLabel() {}

	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjMatrix()) override;

private:
	CFont* Font;
	std::string Text;
	glm::vec3 Colour;
};

