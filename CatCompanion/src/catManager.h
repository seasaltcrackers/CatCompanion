#pragma once
#include <list>
#include <glm.hpp>
#include <chrono>

#include "layer.h"

class CCat;
class CScene;
class CConfigs;
class CConfigSettings;

class CCatManager :
	public ILayer
{
public:
	CCatManager(CScene* parent);
	~CCatManager();

	void Update() override;
	void Render() override;

	void UpdateMouseInput();

	void LoadCats();

	CCat* GetFirstCat();
	void DeleteCat(std::list<CCat*>::iterator it);

	CCat* AddCat(glm::vec2 location, CConfigs* config);
	CCat* AddCat(glm::vec2 location);

	CScene* Parent;
	int NumberOfColours;

private:
	CConfigSettings* CatConfigSettings;
	std::list<CCat*> Cats;

	// Input variables
	enum class MouseState
	{
		Nothing,
		LMB,
		RMB,
	};

	bool RMBHeld;
	MouseState CurrentMouseState;
	std::chrono::steady_clock::time_point RClickTimePoint;
	std::list<CCat*>::iterator CurrentlySelectedCat;


	void CalculateHues();
	bool GetHoveredCat(std::list<CCat*>::iterator& it);
	void GenerateCatConfigSettings();
};

