#include <algorithm>

#include "room.h"
#include "layer.h"

CRoom::CRoom(std::shared_ptr<CCamera> camera)
{
	MainCamera = camera;
}

CRoom::~CRoom()
{
	std::for_each(Layers.begin(), Layers.end(), [](ILayer* aLay) { delete aLay; });
	Layers.clear();
}

void CRoom::UpdateCamera()
{
	MainCamera->Update();
}

void CRoom::Update()
{
	std::for_each(Layers.begin(), Layers.end(), [](ILayer* aLay) { aLay->Update(); });
}

void CRoom::Render()
{
	std::for_each(Layers.begin(), Layers.end(), [](ILayer* aLay) { aLay->Render(); });
}

void CRoom::OnSelect()
{
}
