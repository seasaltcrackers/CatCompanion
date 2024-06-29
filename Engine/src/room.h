#pragma once
#include <vector>
#include <memory>

#include "object.h"

class ILayer;

class CRoom
{
public:
	CRoom(std::shared_ptr<CCamera> camera);
	virtual ~CRoom();
	
	void UpdateCamera();
	virtual void Update();
	virtual void Render();
	virtual void OnSelect();

	std::vector<ILayer*> Layers;

private:
	std::shared_ptr<CCamera> MainCamera;

};
