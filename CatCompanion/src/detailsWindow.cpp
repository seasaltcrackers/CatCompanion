#include <gtc\matrix_transform.hpp>
#include <Windows.h>
#include <math.h>
#include <algorithm>

#include "texture.h"
#include "graphics.h"
#include "camera.h"
#include "input.h"
#include "helper.h"
#include "scene.h"

#include "detailsWindow.h"
#include "coreTilemap.h"
#include "focusManager.h"
#include "scene.h"

#define WINDOW_SCALE 2.0f

Button::Button() :
	MM(&CMonitorManager::GetInstance())
{
	ParentLoc = nullptr;
	UnselectedTex = nullptr;
	SelectedTex = nullptr;
	ButtonBoundry = glm::ivec4(0, 0, 0, 0);
}

Button::Button(glm::ivec2* parentLoc, CTexture* unselected, CTexture* selected, glm::ivec4 bounds) :
	MM(&CMonitorManager::GetInstance())
{
	ParentLoc = parentLoc;
	UnselectedTex = unselected;
	SelectedTex = selected;
	ButtonBoundry = bounds;
}

CTexture* Button::GetTextureID()
{
	return (HELPER::InBounds(glm::ivec2(GINPUT::GetMouseLocation()) - *ParentLoc, ButtonBoundry) ? SelectedTex : UnselectedTex);
}


ToggledButton::ToggledButton()
{
	ToggledUnselectedTex = nullptr;
	ToggledSelectedTex = nullptr;
	Toggled = false;
}

ToggledButton::ToggledButton(glm::ivec2* parentLoc, CTexture* unselected, CTexture* selected, CTexture* toggledUnselected, CTexture* toggledSelected, glm::ivec4 bounds) :
	Button(parentLoc, unselected, selected, bounds)
{
	ToggledUnselectedTex = toggledUnselected;
	ToggledSelectedTex = toggledSelected;
	Toggled = false;
}

CTexture* ToggledButton::GetTextureID()
{
	return (HELPER::InBounds(glm::ivec2(GINPUT::GetMouseLocation()) - *ParentLoc, ButtonBoundry) ? (Toggled ? ToggledSelectedTex : SelectedTex) : (Toggled ? ToggledUnselectedTex : UnselectedTex));
}

CDetailsWindow::CDetailsWindow(std::vector<CCoreTilemap*> tilemaps, glm::ivec2 location) :
	MM(CMonitorManager::GetInstance()), FM(CFocusManager::GetInstance())
{
	Location = location;
	Tilemaps = tilemaps;
	ParentTilemap = nullptr;
	MainState = MouseState::Nothing;
	DragMouseOffset = glm::ivec2();
	TargetObject = nullptr;
	IsVisible = true;
	ItemOffset = 0;

	BaseTexture = CTexture::GetTexture("TileEditorMenu");

	ExitButton = Button(&Location, CTexture::GetTexture("ExitButtonUnselected"), CTexture::GetTexture("ExitButtonSelected"), glm::vec4(126, 0, 135, 9) * WINDOW_SCALE);
	LeftArrow = Button(&Location, CTexture::GetTexture("LeftArrowUnselected"), CTexture::GetTexture("LeftArrowSelected"), glm::vec4(2, 142, 18, 158) * WINDOW_SCALE);
	RightArrow = Button(&Location, CTexture::GetTexture("RightArrowUnselected"), CTexture::GetTexture("RightArrowSelected"), glm::vec4(117, 142, 133, 158) * WINDOW_SCALE);
	RubbishBin = ToggledButton(&Location, CTexture::GetTexture("RubbishBinUnselected"), CTexture::GetTexture("RubbishBinSelected"), CTexture::GetTexture("RubbishBinToggledUnselected"), CTexture::GetTexture("RubbishBinToggledSelected"), glm::vec4(61, 142, 74, 158) * WINDOW_SCALE);
}

void CDetailsWindow::Update()
{
	IfThenReturn(!IsVisible, );

	glm::ivec2 mouseLoc = glm::ivec2(GINPUT::GetMouseLocation());
	for (int i = 0; i < (int)Tilemaps.size(); ++i)
	{
		if (ParentTilemap != Tilemaps[i] && Tilemaps[i]->IsHovering(mouseLoc))
		{
			ParentTilemap = Tilemaps[i];
			break;
		}
	}
	
	bool mouseOnWindow = HELPER::InBounds(mouseLoc, glm::vec2(Location), glm::vec2(Location) + glm::vec2(BaseTexture->GetDimensions()) * WINDOW_SCALE);
	TileObject* selectedObject = ParentTilemap->GetObjectFromPixel(mouseLoc);

	IfThenStatement(MainState != MouseState::Nothing, SetCursor(LoadCursor(NULL, IDC_SIZEALL)));

	switch (GINPUT::GetLMButtonState())
	{
		case (InputState::INPUT_DOWN_FIRST):
		{
			if (mouseOnWindow)
			{
				if (mouseLoc.y <= Location.y + 9 * WINDOW_SCALE)
				{
					if (mouseLoc.x > Location.x + (BaseTexture->GetWidth() - 9) * WINDOW_SCALE)
					{
						// Exit
						SetVisibility(false);
						return;
					}
					else
					{
						// Drag the window
						MainState = MouseState::DraggingWindow;
						DragMouseOffset = mouseLoc - Location;

					}
				}
				else if (HELPER::InBounds(mouseLoc, glm::vec2(Location) + glm::vec2(2, 10) * WINDOW_SCALE, glm::vec2(Location) + (glm::vec2(BaseTexture->GetDimensions()) - glm::vec2(1, 18)) * WINDOW_SCALE))
				{
					glm::vec2 selectedTile = glm::vec2(mouseLoc) - (glm::vec2(Location) + glm::vec2(2, 10) * WINDOW_SCALE);
					const float tileSize = 33.0f * WINDOW_SCALE;

					if (std::fmodf(selectedTile.x, (33 * WINDOW_SCALE)) < 32 * WINDOW_SCALE && std::fmodf(selectedTile.y, (33 * WINDOW_SCALE)) < 32 * WINDOW_SCALE)
					{
						// Select Tile
						selectedTile /= tileSize;
						selectedTile = glm::floor(selectedTile);

						int index = ((int)selectedTile.y * 4 + (int)selectedTile.x) + ItemOffset;

						if (index < (int)CCoreTilemap::TilesTemplates.size())
						{
							TargetObject = new TileObject(ParentTilemap, CCoreTilemap::TilesTemplates[index], ParentTilemap->PixelToCell(mouseLoc));
							MainState = MouseState::PlacingTile;
						}
					}
				}
				else if (HELPER::InBounds(mouseLoc - Location, LeftArrow.ButtonBoundry))
				{
					// Left Button
					ItemOffset = (ItemOffset == 0) ? (16 * (int)std::floorf((float)CCoreTilemap::TilesTemplates.size() / 16.0f)) : (ItemOffset - 16);
				}
				else if (HELPER::InBounds(mouseLoc - Location, RightArrow.ButtonBoundry))
				{
					// Right Button
					ItemOffset = (ItemOffset + 16) % (16 * (int)std::ceilf((float)CCoreTilemap::TilesTemplates.size() / 16.0f));
				}
				else if (HELPER::InBounds(mouseLoc - Location, RubbishBin.ButtonBoundry))
				{
					// Rubbish Bin
					RubbishBin.Toggled = !RubbishBin.Toggled;
				}
			}
			else if (selectedObject != nullptr)
			{
				// Move an existing tile object
				TargetObject = selectedObject;
				OriginalTargetLocation = selectedObject->CellLocation;
				OriginalTargetParent = ParentTilemap;
				DragMouseOffset = ParentTilemap->CellToPixel(ParentTilemap->PixelToCell(mouseLoc)) - ParentTilemap->CellToPixel(selectedObject->CellLocation);
				MainState = MouseState::MovingTile;

				ParentTilemap->RemoveObject(selectedObject);
			}

			break;
		}
		case (InputState::INPUT_UP_FIRST):
		{
			if (RubbishBin.Toggled && MainState == MouseState::MovingTile && OriginalTargetLocation == TargetObject->CellLocation && ParentTilemap == OriginalTargetParent)
			{
				// Failed to place tile
				delete TargetObject;
				TargetObject = nullptr;
			}
			else if (MainState == MouseState::PlacingTile || MainState == MouseState::MovingTile)
			{
				bool isObjectValid = ParentTilemap->IsObjValid(TargetObject);

				if (!mouseOnWindow)
				{
					if (isObjectValid)
					{
						// Successfully placed/moved object
						ParentTilemap->StampObject(TargetObject);
						TargetObject = nullptr;
					}
					else if (MainState == MouseState::MovingTile)
					{
						// Failed to move tile
						TargetObject->ParentTilemap = OriginalTargetParent;
						TargetObject->SetCellLocation(OriginalTargetLocation);
						OriginalTargetParent->StampObject(TargetObject);
						TargetObject = nullptr;
					}
					else
					{
						// Failed to place tile
						delete TargetObject;
						TargetObject = nullptr;
					}
				}
				else
				{
					// Placed back on window
					delete TargetObject;
					TargetObject = nullptr;
				}
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));
			MainState = MouseState::Nothing;
			break;
		}
	}

	switch (MainState)
	{
		case MouseState::DraggingWindow:
		{
			Location = mouseLoc - DragMouseOffset;
			break;
		}
		case MouseState::PlacingTile:
		{
			glm::ivec2 newCellLoc = ParentTilemap->PixelToCell(mouseLoc);
			TargetObject->ParentTilemap = ParentTilemap;
			TargetObject->SetCellLocation(newCellLoc);
			break;
		}
		case MouseState::MovingTile:
		{
			glm::ivec2 newCellLoc = ParentTilemap->PixelToCell(mouseLoc - DragMouseOffset);
			TargetObject->ParentTilemap = ParentTilemap;
			TargetObject->SetCellLocation(newCellLoc);
			break;
		}
	}

	Location = glm::clamp(glm::vec2(Location), glm::zero< glm::vec2>(), glm::vec2(CCamera::GetScreenWidth(), CCamera::GetScreenHeight()) - glm::vec2(BaseTexture->GetDimensions()) * WINDOW_SCALE);

	// Set the windows focus
	bool mouseOnObject = (selectedObject != nullptr);

	if (mouseOnWindow || mouseOnObject)
	{
		FM.SetFocused();
	}
}

void CDetailsWindow::Draw()
{
	IfThenReturn(!IsVisible, );

	glm::mat4 modelMatBase = GFX::CalculateModelMatrix(Location, glm::vec2(BaseTexture->GetWidth() * WINDOW_SCALE, BaseTexture->GetHeight() * WINDOW_SCALE));
	glm::mat4 pvmBase = CCamera::GetProjViewMatrix() * modelMatBase;
	GFX::DrawImage(BaseTexture->GetID(), pvmBase);

	glm::mat4 modelMatLeft = GFX::CalculateModelMatrix(glm::ivec2(LeftArrow.ButtonBoundry) + Location, glm::vec2(LeftArrow.SelectedTex->GetDimensions()) * WINDOW_SCALE);
	glm::mat4 pvmLeft = CCamera::GetProjViewMatrix() * modelMatLeft;
	GFX::DrawImage(LeftArrow.GetTextureID()->GetID(), pvmLeft);

	glm::mat4 modelMatRight = GFX::CalculateModelMatrix(glm::ivec2(RightArrow.ButtonBoundry) + Location, glm::vec2(RightArrow.SelectedTex->GetDimensions()) * WINDOW_SCALE);
	glm::mat4 pvmRight = CCamera::GetProjViewMatrix() * modelMatRight;
	GFX::DrawImage(RightArrow.GetTextureID()->GetID(), pvmRight);

	glm::mat4 modelMatRubbish = GFX::CalculateModelMatrix(glm::ivec2(RubbishBin.ButtonBoundry) + Location, glm::vec2(RubbishBin.SelectedTex->GetDimensions()) * WINDOW_SCALE);
	glm::mat4 pvmRubbish = CCamera::GetProjViewMatrix() * modelMatRubbish;
	GFX::DrawImage(RubbishBin.GetTextureID()->GetID(), pvmRubbish);

	glm::mat4 modelMatExit = GFX::CalculateModelMatrix(glm::ivec2(ExitButton.ButtonBoundry) + Location, glm::vec2(ExitButton.SelectedTex->GetDimensions()) * WINDOW_SCALE);
	glm::mat4 pvmExit = CCamera::GetProjViewMatrix() * modelMatExit;
	GFX::DrawImage(ExitButton.GetTextureID()->GetID(), pvmExit);


	for (int i = 0; i < 16; ++i)
	{
		if (i + ItemOffset < (int)CCoreTilemap::TilesTemplates.size())
		{
			TileTemplate* aTemplate = CCoreTilemap::TilesTemplates[i + ItemOffset];
			glm::mat4 modelMatTemplate;

			glm::vec2 scale = (aTemplate->TexLocSize.z > aTemplate->TexLocSize.w ?
							   glm::vec2(25.0f * WINDOW_SCALE, (25 * WINDOW_SCALE) * ((float)aTemplate->TexLocSize.w / (float)aTemplate->TexLocSize.z)) :
							   glm::vec2((25.0f * WINDOW_SCALE) * ((float)aTemplate->TexLocSize.z / (float)aTemplate->TexLocSize.w), 25.0f * WINDOW_SCALE));

			if (aTemplate->TexLocSize.z * WINDOW_SCALE < scale.x && aTemplate->TexLocSize.w * WINDOW_SCALE < scale.y)
			{
				scale = glm::vec2(aTemplate->TexLocSize.z, aTemplate->TexLocSize.w) * WINDOW_SCALE;
			}

			glm::vec2 location = glm::vec2(Location) + glm::vec2(2, 10) * WINDOW_SCALE + glm::vec2(33.0f * WINDOW_SCALE, 33.0f * WINDOW_SCALE) * glm::vec2(fmod(i, 4), floorf((float)i / 4.0f));
			modelMatTemplate = GFX::CalculateModelMatrix(location + glm::vec2(16.0f * WINDOW_SCALE, 16.0f * WINDOW_SCALE) - scale * 0.5f, scale);

			glm::mat4 pvmTemplate = CCamera::GetProjViewMatrix() * modelMatTemplate;
			GFX::DrawSubImage(ParentTilemap->GetTileTexture(), pvmTemplate, aTemplate->TexLocSize);
		}
	}

	if (MainState == MouseState::PlacingTile ||
		MainState == MouseState::MovingTile)
	{
		glm::mat4 modelMatObj = TargetObject->ModelMatrix;
		glm::mat4 pvmObj = CCamera::GetProjViewMatrix() * modelMatObj;
		GFX::DrawSubImage(ParentTilemap->GetTileTexture(), pvmObj, TargetObject->Parent->TexLocSize);

		glm::ivec2 startLoc = TargetObject->CellLocation + TargetObject->Parent->MaskOffset;
		glm::ivec2 maskSize = TargetObject->Parent->MaskSize;

		glm::vec4 colour = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);

		for (int x = 0; x < maskSize.x; ++x)
		{
			for (int y = 0; y < maskSize.y; ++y)
			{
				if (TargetObject->Parent->GetMaskValue(x, y))
				{
					glm::ivec2 tempLoc = startLoc + glm::ivec2(x, y);

					if (ParentTilemap->GetObjectFromCell(tempLoc) != nullptr)
					{
						glm::vec2 aLoc = ParentTilemap->CellToPixel(tempLoc);
						glm::vec2 aSize = glm::vec2(TileSize * WINDOW_SCALE, TileSize * WINDOW_SCALE);
						GFX::DrawRectangle(CCamera::GetProjViewMatrix(), colour, aLoc, aSize);
					}
				}
			}
		}
	}
}

void CDetailsWindow::SetVisibility(bool newValue)
{
	IsVisible = newValue;
}

