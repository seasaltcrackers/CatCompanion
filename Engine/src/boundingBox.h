#pragma once
#include <glm.hpp>
#include <Windows.h>

class CTilemapLayer;

class CBoundingBox
{
public:
	CBoundingBox(glm::vec2 aLoc, float aWidth, float aHeight);
	
	float left;
	float top;
	float right;
	float bottom;

	void SetWidth(float w);
	void SetHeight(float h);

	void SetLocation(glm::vec2 aLoc);
	void AddLocation(glm::vec2 aLoc);

	void UpdateTransform(glm::vec2 aLoc, float w, float h);
	
	static void CheckMove(CTilemapLayer* aTilemap, CBoundingBox* bbox, glm::vec2& move);
private:
	float GetWidth();
	float GetHeight();

	glm::vec2 Location;

};

