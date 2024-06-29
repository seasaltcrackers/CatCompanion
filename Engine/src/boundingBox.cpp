#include "boundingBox.h"
#include "tilemapLayer.h"

CBoundingBox::CBoundingBox(glm::vec2 aLoc, float aWidth, float aHeight)
{
	Location = aLoc;
	SetWidth(aWidth);
	SetHeight(aHeight);
}

void CBoundingBox::SetWidth(float w)
{
	w *= 0.5f;

	left = Location.x - w;
	right = Location.x + w;
}

void CBoundingBox::SetHeight(float h)
{
	h *= 0.5f;

	top = Location.y - h;
	bottom = Location.y + h;
}

void CBoundingBox::SetLocation(glm::vec2 aLoc)
{
	Location = aLoc;

	float w = GetWidth() * 0.5f;
	float h = GetHeight() * 0.5f;

	left = aLoc.x - w;
	right = aLoc.x + w;

	top = aLoc.y - h;
	bottom = aLoc.y + h;
}

void CBoundingBox::AddLocation(glm::vec2 aLoc)
{
	Location += aLoc;

	left += aLoc.x;
	right += aLoc.x;

	top += aLoc.y;
	bottom += aLoc.y;
}

void CBoundingBox::UpdateTransform(glm::vec2 aLoc, float w, float h)
{
	Location = aLoc;

	w *= 0.5f;
	h *= 0.5f;

	left = aLoc.x - w;
	right = aLoc.x + w;

	top = aLoc.y - h;
	bottom = aLoc.y + h;
}

void CBoundingBox::CheckMove(CTilemapLayer* aTilemap, CBoundingBox* bbox, glm::vec2& move)
{
	if (move.x != 0)
	{
		float point = (move.x > 0.0f ? bbox->right : bbox->left);
		int newPoint = (int)(std::roundf(point + move.x));

		if (aTilemap->GetCellFromPixel(newPoint, (int)bbox->top) != 0 ||
			aTilemap->GetCellFromPixel(newPoint, (int)bbox->bottom) != 0)
		{
			move.x = (float)(newPoint - (aTilemap->GetCellXFromPixel(newPoint) + (move.x < 0.0f)) * aTilemap->GetTileWidth());
		}
	}

	if (move.y != 0)
	{
		float point = (move.y > 0 ? bbox->bottom : bbox->top);
		int newPoint = (int)(point + move.y);

		if (aTilemap->GetCellFromPixel((int)bbox->left, newPoint) != 0.0f ||
			aTilemap->GetCellFromPixel((int)bbox->right, newPoint) != 0.0f)
		{
			move.y = (float)(newPoint - (aTilemap->GetCellYFromPixel(newPoint) + (move.y < 0)) * aTilemap->GetTileWidth());
		}
	}
}

float CBoundingBox::GetWidth()
{
	return right - left;
}

float CBoundingBox::GetHeight()
{
	return bottom - top;
}
