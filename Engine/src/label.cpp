#include <iostream>

#include "label.h"
#include "graphics.h"
#include "font.h"

CLabel::CLabel(CFont* font, std::string text, glm::vec2 location, glm::ivec3 colour)
{
	Location = glm::vec3(location, 0.0f);
	Colour = glm::vec3(colour) / 255.0f;
	Font = font;
	Text = text;
}

void CLabel::Update()
{
	// Do nothing
}

void CLabel::Draw(glm::mat4& pv)
{
	GFX::DrawString(pv, Font, Text, glm::vec2(Location), Colour, 1.0f);
}
