#include "button.h"
#include "texture.h"
#include "program.h"
#include "mesh.h"
#include "input.h"
#include "helper.h"
#include "graphics.h"
#include "camera.h"
#include "font.h"

CButton::CButton(CFont* font, std::string text, glm::vec3 colour, glm::ivec2 location, glm::ivec2 offset, CTexture* texture) :
	CObject(CProgram::GetProgram("Texture"), CMesh::GetMesh("2D Square"))
{
	TextFont = font;
	TextureOffset = offset;
	Textures.push_back(texture);
	SetLocation(location);
	SetTexture(0);
	SetText(text);
	SetColour(colour);
}

CButton::CButton(CFont* font, std::string text, glm::vec3 colour, glm::ivec2 location, glm::ivec2 offset, std::vector<CTexture*> textures) :
	CObject(CProgram::GetProgram("Texture"), CMesh::GetMesh("2D Square"))
{
	TextFont = font;
	TextureOffset = offset;
	Textures = textures;
	SetLocation(location);
	SetTexture(0);
	SetText(text);
	SetColour(colour);
}

CButton::CButton(CFont* font, std::string text, glm::vec3 colour, glm::ivec2 location)
{
	TextFont = font;
	SetLocation(location);
	SetText(text);
	SetColour(colour);
	UpdateBounds();
}

void CButton::SetTexture(int aTex)
{
	glm::vec2 oldScale = glm::vec2(Scale.x, Scale.y);
	IfThenStatement(CurrentTexture != nullptr, oldScale /= CurrentTexture->GetDimensions());

	CurrentTextureID = aTex;
	CurrentTexture = Textures[aTex];
	Scale = glm::vec3(glm::vec2(CurrentTexture->GetDimensions()) * oldScale, 1.0f);
	UpdateUniformTexture("tex", CurrentTexture->GetID());
	UpdateBounds();
}

void CButton::SetColour(glm::vec3 aColour)
{
	TextColour = aColour / glm::vec3(255.0f, 255.0f, 255.0f);
}

void CButton::SetText(std::string aString)
{
	Text = aString;
	UpdateBounds();
}

void CButton::SetLocation(glm::ivec2 aLoc)
{
	TextLocation = aLoc;
	UpdateBounds();
	oldLocation = Location;
}

void CButton::SetTextAlign(HAlign textHAlign)
{
	TextHAlign = textHAlign;
	UpdateBounds();
}

void CButton::SetTextAlign(VAlign textVAlign)
{
	TextVAlign = textVAlign;
	UpdateBounds();
}

void CButton::SetIsVisible(bool newVisibility)
{
	IsVisible = newVisibility;
}

glm::vec3 CButton::GetColour()
{
	return TextColour * glm::vec3(255.0f, 255.0f, 255.0f);
}

int CButton::GetCurrentTexture()
{
	return CurrentTextureID;
}

void CButton::SetOnPress(std::function<void()> aFunc)
{
	OnPress = aFunc;
}

void CButton::SetOnEnter(std::function<void()> aFunc)
{
	OnEnter = aFunc;
}

void CButton::SetOnLeave(std::function<void()> aFunc)
{
	OnLeave = aFunc;
}

void CButton::SetOnHover(std::function<void()> aFunc)
{
	OnHover = aFunc;
}

void CButton::SetOnIdle(std::function<void()> aFunc)
{
	OnIdle = aFunc;
}

void CButton::Update()
{
	if (oldLocation != Location)
	{
		oldLocation = Location;
		TextLocation = glm::ivec2(Location);
		UpdateBounds();
	}

	IfThenReturn(!IsVisible,);
	CObject::Update();

	bool newInBounds = HELPER::InBounds(GINPUT::GetMouseLocation(), Bounds);

	if (newInBounds)
	{
		IfThenStatement(OnHover != NULL, OnHover());

		if (!OldInBounds)
		{
			IfThenStatement(OnEnter != NULL, OnEnter());
		}

		if (GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST)
		{
			IfThenStatement(OnPress != NULL, OnPress());
		}
	}
	else if (OldInBounds)
	{
		IfThenStatement(OnLeave != NULL, OnLeave());
	}
	else
	{
		IfThenStatement(OnIdle != NULL, OnIdle());
	}

	OldInBounds = newInBounds;
}

void CButton::Draw(glm::mat4& pv)
{
	IfThenReturn(!IsVisible,);
	IfThenStatement(CurrentTexture != nullptr, CObject::Draw(CCamera::GetProjMatrix()));

	glm::ivec2 tempBounds = TextFont->TextBounds(Text, 1.0f);
	int tempX = (int)std::roundf(TextLocation.x - tempBounds.x * (float)TextHAlign * 0.5f);
	int tempY = (int)std::roundf(TextLocation.y - tempBounds.y * (float)TextVAlign * 0.5f);

	GFX::DrawString(pv, TextFont, Text, glm::ivec2(tempX, tempY), TextColour);
}

void CButton::UpdateBounds()
{
	if (CurrentTexture == nullptr)
	{
		glm::ivec2 tempBounds = TextFont->TextBounds(Text, 1.0f);
		Bounds.x = (int)std::roundf(TextLocation.x - tempBounds.x * (float)TextHAlign * 0.5f * Scale.x);
		Bounds.y = (int)std::roundf(TextLocation.y - tempBounds.y * (float)TextVAlign * 0.5f * Scale.y);
		Bounds.z = Bounds.x + tempBounds.x * Scale.x;
		Bounds.w = Bounds.y + tempBounds.y * Scale.y;

		Location.x = (float)Bounds.x;
		Location.y = (float)Bounds.y;
	}
	else
	{
		float scaleX = Scale.x / CurrentTexture->GetDimensions().x;
		float scaleY = Scale.y / CurrentTexture->GetDimensions().y;

		Bounds.x = (int)std::roundf(TextLocation.x + TextureOffset.x - Pivot.x * CurrentTexture->GetWidth() * scaleX);
		Bounds.y = (int)std::roundf(TextLocation.y + TextureOffset.y - Pivot.y * CurrentTexture->GetHeight() * scaleY);
		Bounds.z = Bounds.x + CurrentTexture->GetWidth() * scaleX;
		Bounds.w = Bounds.y + CurrentTexture->GetHeight() * scaleY;

		Location.x = Bounds.x + Pivot.x * CurrentTexture->GetWidth() * scaleX;
		Location.y = Bounds.y + Pivot.y * CurrentTexture->GetHeight() * scaleY;
	}

}
