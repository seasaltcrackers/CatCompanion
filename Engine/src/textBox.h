#pragma once
#include "object.h"

class CFont;

class CTextBox :
	public CObject
{
public:
	CTextBox(CFont* aFont, glm::vec2 location, int width, glm::vec3 boxColour = glm::one<glm::vec3>(), glm::vec3 textColour = glm::zero<glm::vec3>());
	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;

	std::string GetText();
	void SetText(std::string text);

private:
	std::vector<unsigned int>& KeysPressed;

	CFont* Font;
	std::string Text;
	int Width;
	int Margin;

	glm::vec3 BoxColour;
	glm::vec3 TextColour;

	glm::vec4 Boundry;

	bool IsSelected = false;
	bool SelectionBarVisible = false;
	float SelectionBarTime = 0.0f;
	const float SelectionBarInterval = 0.5f;
};

