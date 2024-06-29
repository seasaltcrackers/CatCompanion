#include "textBox.h"
#include "input.h"
#include "graphics.h"
#include "font.h"
#include "helper.h"
#include "input.h"
#include "timer.h"

CTextBox::CTextBox(CFont* aFont, glm::vec2 location, int width, glm::vec3 boxColour, glm::vec3 textColour) :
	CObject(nullptr, nullptr), KeysPressed(GINPUT::GetUpdatedKeys())
{
	Margin = 2.0f;

	Font = aFont;
	Width = width;
	Boundry = glm::vec4(location.x, location.y, width, aFont->Size * 0.5f);

	BoxColour = boxColour;
	TextColour = textColour;

	Location = glm::vec3(location, 0.0f);
}


void CTextBox::Update()
{
	bool isInBounds = HELPER::InBounds(GINPUT::GetMouseLocation(), glm::vec2(Location), glm::vec2(Location) + glm::vec2(Boundry.z, Boundry.w));
	bool isClicked = GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST;

	if (IsSelected)
	{
		int size = Text.size();

		for (int i = 0; i < (int)KeysPressed.size(); ++i)
		{
			char key = KeysPressed[i];

			// Check if it is valid input
			if (GINPUT::GetKeyInputState(key) == InputState::INPUT_DOWN_FIRST)
			{
				if (((key >= 'a' && key <= 'z') ||
					(key >= 'A' && key <= 'Z') ||
					 key == ' '))
				{
					Text += std::toupper(key);

					glm::ivec2 bounds = Font->TextBounds(Text, 1.0f);

					if (bounds.x > Width)
					{
						Text = Text.substr(0, Text.size() - 1);
					}
				}
				else if (key == VK_BACK && Text.size() > 0)
				{
					Text = Text.substr(0, Text.size() - 1);
				}
			}
		}

		if (size != Text.size())
		{
			SelectionBarVisible = true;
			SelectionBarTime = 0.0f;
		}

		if (!isInBounds && isClicked)
		{
			IsSelected = false;
		}
	}
	else
	{
		if (isInBounds && isClicked)
		{
			IsSelected = true;
			
			SelectionBarVisible = true;
			SelectionBarTime = 0.0f;
		}
	}

	if (IsSelected)
	{
		SelectionBarTime += TIMER::GetDeltaSeconds();
		
		if (SelectionBarTime >= SelectionBarInterval)
		{
			SelectionBarTime -= SelectionBarInterval;
			SelectionBarVisible = !SelectionBarVisible;
		}
	}

	CObject::Update();
}

void CTextBox::Draw(glm::mat4& pv)
{
	GFX::DrawRectangle(pv, BoxColour, glm::vec2(Location), glm::vec2(Boundry.z, Boundry.w));
	GFX::DrawString(pv, Font, Text, glm::vec2(Location) + glm::vec2(Margin, Margin), TextColour);

	if (IsSelected && SelectionBarVisible)
	{
		glm::ivec2 textDimensions = Font->TextBounds(Text, 1.0f);
		GFX::DrawRectangle(pv, TextColour, glm::vec2(textDimensions.x, 0.0f) + glm::vec2(Location) + glm::vec2(Margin, Margin), glm::vec2(4.0f, Boundry.w - Margin * 2.0f));
	}
}

std::string CTextBox::GetText()
{
	return Text;
}


void CTextBox::SetText(std::string text)
{
	Text = text;
}