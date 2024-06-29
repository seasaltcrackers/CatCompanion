#pragma once
#include <glm.hpp>
#include "instanceLayer.h"

class CGUILayer :
	public CInstanceLayer
{
public:
	CGUILayer();

	virtual void Render() override;

private:
	glm::mat4 ProjectionMatrix;
};

