#pragma once
#include <vector>

#include "layer.h"

class CObject;

class CInstanceLayer :
	public ILayer
{
public:
	CInstanceLayer();
	virtual ~CInstanceLayer();

	virtual void Render() override;
	virtual void Update() override;

	void ClearObjects();
	void SetVisibility(bool visibility);

	std::vector<CObject*> Objects;

private:
	bool IsVisible = true;
};

