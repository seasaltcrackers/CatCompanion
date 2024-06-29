#pragma once
#include <iostream>

#include "object.h"
#include "helper.h"

struct Boundry
{
	virtual float GetWidth() { return 0.0f; }
	virtual float GetHeight() { return 0.0f; }

	virtual bool Intersecting(glm::vec2 point) { return false; }
	virtual bool Intersecting(float x, float y) { return false; }

	virtual void Render(glm::vec2 offset) {}
};

struct BoundryPtWH;

struct BoundryPtToPt :
	Boundry
{
	glm::vec2 Point1;
	glm::vec2 Point2;

	BoundryPtToPt(BoundryPtWH bounds);
	BoundryPtToPt(glm::vec2 p1, glm::vec2 p2);
	BoundryPtToPt(float x1, float y1, float x2, float y2);

	float GetWidth() override;
	float GetHeight() override;

	bool Intersecting(glm::vec2 point) override;
	bool Intersecting(float x, float y) override;

	void Render(glm::vec2 offset) override;
};

struct BoundryPtWH :
	Boundry
{
	glm::vec2 Location;
	glm::vec2 Dimension;

	BoundryPtWH(BoundryPtToPt bounds);
	BoundryPtWH(glm::vec2 location, glm::vec2 dimension);
	BoundryPtWH(float x, float y, float width, float height);

	float GetWidth() override;
	float GetHeight() override;

	bool Intersecting(glm::vec2 point) override;
	bool Intersecting(float x, float y) override;

	void Render(glm::vec2 offset) override;
};

class CGuiControl :
	public CObject
{
public:
	CGuiControl(glm::vec2 location, std::shared_ptr<Boundry> collisionMask, CGuiControl* parent = nullptr);
	virtual ~CGuiControl();

	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjMatrix()) override;

	bool GetIsVisible();
	void SetIsVisible(bool isVisible);

	bool GetIsEnabled();
	void SetIsEnabled(bool isEnabled);

	void SetVAlign(VAlign vAlign);
	void SetHAlign(HAlign hAlign);

protected:
	bool IsVisible;
	bool IsEnabled;

	std::shared_ptr<Boundry> CollisionMask;
	std::shared_ptr<Boundry> GuiMask;

	HAlign HAlignment;
	VAlign VAlignment;

	CGuiControl* Parent;
	std::vector<CGuiControl*> Children;

	glm::vec2 LocalPosition;
	glm::vec2 WorldPosition;

	virtual void OnUpdate() { }
	virtual void OnRender() { }

	virtual void OnMouseDown() { }
	virtual void OnMouseUp() { }
	virtual void OnMouseClick() { }

	virtual void OnMouseEnter() { }
	virtual void OnMouseLeave() { }

	virtual void OnIdle() { }
	virtual void OnMouseHover() { }

private:
	bool WasInBounds;
	bool CanClick;

	glm::vec2 CalculateOffset();
};

