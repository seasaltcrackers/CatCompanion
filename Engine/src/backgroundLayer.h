#pragma once
#include "layer.h"

class CTexture;

class CBackgroundLayer :
	public ILayer
{
public:
	CBackgroundLayer(CTexture* aTexture);

	void Render() override;
	void Update() override;

private:
	CTexture* BackgroundTexture;
};

