#include <algorithm>

#include "instanceLayer.h"
#include "object.h"
#include "helper.h"

CInstanceLayer::CInstanceLayer()
{
	LayerType = "CInstanceLayer";
}

CInstanceLayer::~CInstanceLayer()
{
	ClearObjects();
}

void CInstanceLayer::Render()
{
	IfThenReturn(!IsVisible,);
	std::for_each(Objects.begin(), Objects.end(), [](CObject* obj) { obj->Draw(); });
}

void CInstanceLayer::Update()
{
	IfThenReturn(!IsVisible,);
	std::for_each(Objects.begin(), Objects.end(), [](CObject* obj) { obj->Update(); });
}

void CInstanceLayer::ClearObjects()
{
	std::for_each(Objects.begin(), Objects.end(), [](CObject* obj) { delete obj; });
	Objects.clear();
}

void CInstanceLayer::SetVisibility(bool visibility)
{
	IsVisible = visibility;
}
