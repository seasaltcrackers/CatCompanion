#pragma once
#include <string>

class ILayer
{
public:
	virtual ~ILayer() {}

	virtual void Update() = 0;
	virtual void Render() = 0;
	std::string GetLayerType() { return LayerType; }

protected:
	std::string LayerType;
};

