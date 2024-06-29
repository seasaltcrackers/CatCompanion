// Library Includes
#include <algorithm>

// This Include
#include "guiControl.h"

// Local Includes
#include "input.h"
#include "graphics.h"

BoundryPtToPt::BoundryPtToPt(BoundryPtWH bounds)
{
	Point1 = bounds.Location;
	Point2 = bounds.Location + bounds.Dimension;
}

BoundryPtToPt::BoundryPtToPt(glm::vec2 p1, glm::vec2 p2)
{
	Point1 = p1;
	Point2 = p2;
}

BoundryPtToPt::BoundryPtToPt(float x1, float y1, float x2, float y2)
{
	Point1 = { x1, y1 };
	Point2 = { x2, y2 };
}

float BoundryPtToPt::GetWidth()
{
	return Point2.x - Point1.x;
}

float BoundryPtToPt::GetHeight()
{
	return Point2.y - Point1.y;
}

bool BoundryPtToPt::Intersecting(glm::vec2 point)
{
	return (point.x >= Point1.x && point.y >= Point1.y &&
			point.x < Point2.x && point.y < Point2.y);
}

bool BoundryPtToPt::Intersecting(float x, float y)
{
	return (x >= Point1.x && y >= Point1.y &&
			x < Point2.x && y < Point2.y);
}

void BoundryPtToPt::Render(glm::vec2 offset)
{
	GFX::DrawRectangle(CCamera::GetProjMatrix(), { 1.0f, 1.0f, 1.0f }, Point1 + offset, Point2 - Point1);
}

BoundryPtWH::BoundryPtWH(BoundryPtToPt bounds)
{
	Location = bounds.Point1;
	Dimension = bounds.Point2 - bounds.Point1;
}

BoundryPtWH::BoundryPtWH(glm::vec2 location, glm::vec2 dimension)
{
	Location = location;
	Dimension = dimension;
}

BoundryPtWH::BoundryPtWH(float x, float y, float width, float height)
{
	Location = { x, y };
	Dimension = { width, height };
}

float BoundryPtWH::GetWidth()
{
	return Dimension.x;
}

float BoundryPtWH::GetHeight()
{
	return Dimension.y;
}

bool BoundryPtWH::Intersecting(glm::vec2 point)
{
	return (point.x >= Location.x && point.y >= Location.y &&
			point.x < Location.x + Dimension.x && point.y < Location.y + Dimension.y);
}

bool BoundryPtWH::Intersecting(float x, float y)
{
	return (x >= Location.x && y >= Location.y &&
			x < Location.x + Dimension.x && y < Location.y + Dimension.y);
}

void BoundryPtWH::Render(glm::vec2 offset)
{
	GFX::DrawRectangle(CCamera::GetProjMatrix(), { 1.0f, 1.0f, 1.0f }, Location + offset, Dimension);
}

CGuiControl::CGuiControl(glm::vec2 location, std::shared_ptr<Boundry> collisionMask, CGuiControl* parent)
{
	LocalPosition = location;
	CollisionMask = collisionMask;
	Parent = parent;

	IsVisible = true;
	IsEnabled = true;
	WasInBounds = false;
	CanClick = false;

	VAlignment = VAlign::Top;
	HAlignment = HAlign::Left;

	WorldPosition = LocalPosition;
	GuiMask = CollisionMask;
}

CGuiControl::~CGuiControl()
{
	std::for_each(Children.begin(), Children.end(), [](CGuiControl* ctrl) { delete ctrl; });
	Children.clear();
}

void CGuiControl::Update()
{
	IfThenReturn(!IsEnabled || !IsVisible, );

	glm::vec2 parentPos = Parent != nullptr ? Parent->WorldPosition : glm::vec2();
	WorldPosition = LocalPosition + parentPos;

	bool isInBounds = CollisionMask->Intersecting(GINPUT::GetMouseLocation() - parentPos - CalculateOffset());

	if (isInBounds)
	{
		if (!WasInBounds)
		{
			OnMouseEnter();
		}

		InputState lbutton = GINPUT::GetLMButtonState();

		if (lbutton == InputState::INPUT_DOWN_FIRST)
		{
			CanClick = true;
			OnMouseDown();
		}
		else if (lbutton == InputState::INPUT_UP_FIRST)
		{
			OnMouseUp();

			if (CanClick)
			{
				OnMouseClick();
			}
		}

		OnMouseHover();
	}
	else
	{
		if (WasInBounds)
		{
			OnMouseLeave();
		}

		if (CanClick && GINPUT::GetLMButtonState() == InputState::INPUT_UP_FIRST)
		{
			CanClick = false;
		}

		OnIdle();
	}

	WasInBounds = isInBounds;

	OnUpdate();
	std::for_each(Children.begin(), Children.end(), [](CGuiControl* ctrl) { ctrl->Update(); });
}

void CGuiControl::Draw(glm::mat4& pv)
{
	IfThenReturn(!IsVisible,);
	CollisionMask->Render(WorldPosition - LocalPosition + CalculateOffset());
	OnRender();
	std::for_each(Children.begin(), Children.end(), [](CGuiControl* ctrl) { ctrl->Draw(); });
}

bool CGuiControl::GetIsVisible()
{
	return IsVisible;
}

void CGuiControl::SetIsVisible(bool isVisible)
{
	IsVisible = isVisible;
}

bool CGuiControl::GetIsEnabled()
{
	return IsEnabled;
}

void CGuiControl::SetIsEnabled(bool isEnabled)
{
	IsEnabled = isEnabled;
}

void CGuiControl::SetVAlign(VAlign vAlign)
{
	VAlignment = vAlign;
}

void CGuiControl::SetHAlign(HAlign hAlign)
{
	HAlignment = hAlign;
}

glm::vec2 CGuiControl::CalculateOffset()
{
	glm::vec2 offset;

	offset.x = GuiMask->GetWidth() * (float)HAlignment * -0.5f;
	offset.y = GuiMask->GetHeight() * (float)VAlignment * -0.5f;

	return offset;
}
