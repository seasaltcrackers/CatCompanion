#pragma once
#include "coreTilemap.h"
#include "monitorManager.h"

#include <glm.hpp>

class CTexture;
class CFocusManager;

enum class MouseState
{
	Nothing,
	DraggingWindow,
	PlacingTile,
	MovingTile,
};

struct Button
{
	glm::ivec2* ParentLoc;
	CTexture* UnselectedTex;
	CTexture* SelectedTex;
	glm::ivec4 ButtonBoundry;
	CMonitorManager* MM;

	Button();
	Button(glm::ivec2* parentLoc, CTexture* unselected, CTexture* selected, glm::ivec4 bounds);
	virtual CTexture* GetTextureID();
};


struct ToggledButton : Button
{
	CTexture* ToggledUnselectedTex;
	CTexture* ToggledSelectedTex;
	bool Toggled;

	ToggledButton();
	ToggledButton(glm::ivec2* parentLoc, CTexture* unselected, CTexture* selected, CTexture* toggledUnselected, CTexture* toggledSelected, glm::ivec4 bounds);
	CTexture* GetTextureID() override;
};

class CDetailsWindow
{
public:
	CDetailsWindow(std::vector<CCoreTilemap*> tilemaps, glm::ivec2 location);

	void Update();
	void Draw();

	void SetVisibility(bool newValue);

private:
	CMonitorManager& MM;
	CFocusManager& FM;

	std::vector<CCoreTilemap*> Tilemaps;
	CCoreTilemap* ParentTilemap;
	glm::ivec2 Location;

	CTexture* BaseTexture;

	Button ExitButton;
	Button LeftArrow;
	Button RightArrow;
	ToggledButton RubbishBin;

	int ItemOffset;

	MouseState MainState;
	glm::ivec2 DragMouseOffset;
	glm::ivec2 OriginalTargetLocation;
	CCoreTilemap* OriginalTargetParent;
	TileObject* TargetObject;

	bool IsVisible;
};