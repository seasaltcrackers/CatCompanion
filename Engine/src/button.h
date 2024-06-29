#pragma once
#include <functional>
#include <string>
#include <glm.hpp>

#include "object.h"
#include "helper.h"

class CTexture;
class CFont;

class CButton :
	public CObject
{
public:
	CButton(CFont* font, std::string text, glm::vec3 colour, glm::ivec2 location, glm::ivec2 offset, CTexture* texture);
	CButton(CFont* font, std::string text, glm::vec3 colour, glm::ivec2 location, glm::ivec2 offset, std::vector<CTexture*> textures);
	CButton(CFont* font, std::string text, glm::vec3 colour, glm::ivec2 location);

	void SetTexture(int aTex);
	void SetColour(glm::vec3 aColour);
	void SetText(std::string aString);
	void SetLocation(glm::ivec2 aLoc);
	void SetTextAlign(HAlign textHAlign);
	void SetTextAlign(VAlign textVAlign);
	void SetIsVisible(bool newVisibility);

	glm::vec3 GetColour();
	int GetCurrentTexture();

	void SetOnPress(std::function<void()> aFunc);
	void SetOnEnter(std::function<void()> aFunc);
	void SetOnLeave(std::function<void()> aFunc);
	void SetOnHover(std::function<void()> aFunc);
	void SetOnIdle(std::function<void()> aFunc);

	void UpdateBounds();
	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjMatrix()) override;

	std::vector<CTexture*> Textures;

private:	
	std::function<void()> OnPress;
	std::function<void()> OnEnter;
	std::function<void()> OnLeave;
	std::function<void()> OnHover;
	std::function<void()> OnIdle;

	bool IsVisible = true;

	glm::ivec4 Bounds;
	bool OldInBounds;

	CTexture* CurrentTexture;
	int CurrentTextureID;

	CFont* TextFont;
	glm::vec3 TextColour;
	std::string Text;

	glm::ivec2 TextLocation;
	glm::ivec2 TextureOffset;
	HAlign TextHAlign;
	VAlign TextVAlign;

	glm::vec3 oldLocation;
};

