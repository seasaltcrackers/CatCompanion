#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <algorithm>

#include "camera.h"
#include "program.h"
#include "mesh.h"
#include "texture.h"
#include "helper.h"

#include "coreTilemap.h"
#include "configs.h"
#include "scene.h"

#define TILEMAP_SCALE 2.0f

std::vector<TileTemplate*> CCoreTilemap::TilesTemplates;

TileObject::TileObject(CCoreTilemap* parentTilemap, TileTemplate* aTemplate, glm::ivec2 cellLoc)
{
	ParentTilemap = parentTilemap;
	Parent = aTemplate;
	SetCellLocation(cellLoc);
}

void TileObject::SetCellLocation(glm::ivec2& newLocation)
{
	CellLocation = newLocation;
	UpdateModelMat();
}

void TileObject::UpdateModelMat()
{
	glm::ivec2 pixelLoc = ParentTilemap->CellToPixel(CellLocation);

	glm::mat4 scaleMat = glm::scale(glm::mat4(), glm::vec3(Parent->TexLocSize.z * TILEMAP_SCALE, Parent->TexLocSize.w * TILEMAP_SCALE, 1.0f));
	glm::mat4 translationMat = glm::translate(glm::mat4(), glm::vec3(glm::vec2(pixelLoc) + glm::vec2(Parent->TexOffset) * TILEMAP_SCALE, 0.0f));

	ModelMatrix = translationMat * scaleMat;
}

CCoreTilemap::CCoreTilemap(int parentMonitor) : MM(CMonitorManager::GetInstance())
{
	ParentMonitor = parentMonitor;
	MonitorInfo details = MM.GetMonitorDetail(parentMonitor);

	glm::vec2 topLeft = MM.DesktopToWindow(glm::vec2(details.BoundryNTB.left, details.BoundryNTB.top));
	glm::vec2 botRight= MM.DesktopToWindow(glm::vec2(details.BoundryNTB.right, details.BoundryNTB.bottom));

	glm::vec4 bounds = glm::vec4(topLeft, botRight);
	float width = bounds.z - bounds.x;
	float height = bounds.w - bounds.y;

	float tilemapWidth = width / (float)(TileSize * TILEMAP_SCALE);
	float tilemapHeight = height / (float)(TileSize * TILEMAP_SCALE);

	Width = (int)ceilf(tilemapWidth);
	Height = (int)ceilf(tilemapHeight);
	Offset = glm::ivec2(-(((float)Width - tilemapWidth) * (float)(TileSize * TILEMAP_SCALE) * 0.5f), -(((float)Height - tilemapHeight) * (float)(TileSize * TILEMAP_SCALE) * 0.5f));
	Offset.x += (int)bounds.x;
	Offset.y += (int)bounds.y;

	BaseOffset = Offset.y;
	Offset.y -= details.TaskBarHeight;

	TileShader = CProgram::GetProgram("BoundrySubTexture");
	TileTexture = CTexture::GetTexture("Tilemap");
	TileMesh = CMesh::Generate2DRectangle(1.0f, 1.0f);

	Tiles = new TileObject * [Width * Height];
	Mask = new TileObject * [Width * Height];
	RestLocations = new RestType[Width * Height];

	for (int i = 0; i < Width * Height; i++)
	{
		// Initialize all elements.
		Tiles[i] = nullptr;
		Mask[i] = nullptr;
		RestLocations[i] = RestType::Empty;
	}
}

CCoreTilemap::~CCoreTilemap()
{
	delete TileMesh;
}

int CCoreTilemap::GetWidth()
{
	return Width;
}

int CCoreTilemap::GetHeight()
{
	return Height;
}

glm::ivec2 CCoreTilemap::GetSize()
{
	return glm::ivec2(Width, Height);
}

GLuint CCoreTilemap::GetTileTexture()
{
	return TileTexture->GetID();
}

glm::ivec2 CCoreTilemap::GetOffset()
{
	return Offset;
}

void CCoreTilemap::Render()
{
	// Pre-calculate the view proj matrix
	glm::mat4 viewProjMat = CCamera::GetProjViewMatrix();

	// Bind program
	glUseProgram(TileShader->GetProgramID());

	// Bind mesh
	glBindVertexArray(TileMesh->GetVAO());
	int tempIndicieCount = TileMesh->GetIndicieCount();

	// Set Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TileTexture->GetID());
	glUniform1i(TileShader->GetUniform("tex"), 0);

	// Set Boundry
	RECT rec = MM.GetMonitorDetail(ParentMonitor).BoundryNTB;
	
	glm::vec2 topLeft = MM.DesktopToWindow(glm::vec2(rec.left, rec.top));
	glm::vec2 botRight = MM.DesktopToWindow(glm::vec2(rec.right, rec.bottom));

	glm::vec4 bounds = glm::vec4(topLeft, botRight);

	glm::vec4 unitBounds = bounds / glm::vec4(CCamera::GetScreenSize(), CCamera::GetScreenSize()) - glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
	unitBounds *= 2.0f;
	unitBounds.y *= -1.0f;
	unitBounds.w *= -1.0f;

	glUniform4f(TileShader->GetUniform("boundry"), unitBounds.x, unitBounds.y, unitBounds.z, unitBounds.w);

	GLuint pvmID = TileShader->GetUniform("PVM");
	GLuint rectID = TileShader->GetUniform("rect");

	for (int i = 0; i < Width * Height; ++i)
	{
		TileObject* anObject = Tiles[i];

		if (anObject != nullptr)
		{
			glm::mat4 pvm = viewProjMat * anObject->ModelMatrix;
			glUniformMatrix4fv(pvmID, 1, GL_FALSE, glm::value_ptr(pvm));

			glm::ivec4 rect = anObject->Parent->TexLocSize;
			glUniform4i(rectID, rect.x, rect.y, rect.z, rect.w);

			glDrawElements(GL_TRIANGLES, tempIndicieCount, GL_UNSIGNED_INT, 0);
		}
	}

	// Unbind mesh
	glBindVertexArray(0);

	// Unbind Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind program
	glUseProgram(0);
}

void CCoreTilemap::Update()
{
	int newOffset = BaseOffset - MM.GetMonitorDetail(ParentMonitor).TaskBarHeight;

	if (Offset.y != newOffset)
	{
		Offset.y = newOffset;

		for (int i = 0; i < Width * Height; ++i)
		{
			TileObject* anObject = Tiles[i];

			if (anObject != nullptr)
			{
				anObject->UpdateModelMat();
			}
		}
	}
}

glm::ivec2 CCoreTilemap::PixelToCell(glm::ivec2 aLocation)
{
	return glm::floor((glm::vec2(aLocation - Offset) / (float)(TileSize * TILEMAP_SCALE)));
}

glm::ivec2 CCoreTilemap::CellToPixel(glm::ivec2 aLocation)
{
	return glm::vec2(aLocation) * (float)TileSize * TILEMAP_SCALE + glm::vec2(Offset);
}

TileObject* CCoreTilemap::GetObjectFromCell(glm::ivec2 cellLocation)
{
	return (HELPER::InBounds(cellLocation, glm::ivec2(), GetSize()) ? Mask[cellLocation.y * Width + cellLocation.x] : nullptr);
}

TileObject* CCoreTilemap::GetObjectFromPixel(glm::ivec2 pixelLocation)
{
	return GetObjectFromCell(PixelToCell(pixelLocation));
}

bool CCoreTilemap::IsObjValid(TileObject* anObj)
{
	glm::ivec2 startLoc = anObj->CellLocation + anObj->Parent->MaskOffset;

	for (int x = 0; x < anObj->Parent->MaskSize.x; ++x)
	{
		for (int y = 0; y < anObj->Parent->MaskSize.y; ++y)
		{
			glm::ivec2 mapCoords = startLoc + glm::ivec2(x, y);

			if (!HELPER::InBounds(mapCoords.x, mapCoords.y, 0, 0, Width, Height) ||
				(anObj->Parent->GetMaskValue(x, y) &&
				 GetObjectFromCell(mapCoords) != nullptr))
			{
				return false;
			}
		}
	}

	return true;
}

void CCoreTilemap::StampObject(TileObject* anObj)
{
	Tiles[anObj->CellLocation.y * Width + anObj->CellLocation.x] = anObj;
	glm::ivec2 startLoc = anObj->CellLocation + anObj->Parent->MaskOffset;

	for (int x = 0; x < anObj->Parent->MaskSize.x; ++x)
	{
		for (int y = 0; y < anObj->Parent->MaskSize.y; ++y)
		{
			glm::ivec2 mapCoords = startLoc + glm::ivec2(x, y);
			int mapCoordsIndex = mapCoords.y * Width + mapCoords.x;
			Mask[mapCoordsIndex] = (anObj->Parent->GetMaskValue(x, y) ? anObj : Mask[mapCoordsIndex]);
			RestLocations[mapCoordsIndex] = (anObj->Parent->GetRestValue(x, y) != RestType::Empty ? anObj->Parent->GetRestValue(x, y) : RestLocations[mapCoordsIndex]);
		}
	}
}

void CCoreTilemap::RemoveObject(TileObject* anObj)
{
	Tiles[anObj->CellLocation.y * Width + anObj->CellLocation.x] = nullptr;
	glm::ivec2 startLoc = anObj->CellLocation + anObj->Parent->MaskOffset;

	for (int x = 0; x < anObj->Parent->MaskSize.x; ++x)
	{
		for (int y = 0; y < anObj->Parent->MaskSize.y; ++y)
		{
			glm::ivec2 mapCoords = startLoc + glm::ivec2(x, y);
			int mapCoordsIndex = mapCoords.y * Width + mapCoords.x;
			Mask[mapCoordsIndex] = (anObj->Parent->GetMaskValue(x, y) ? nullptr : Mask[mapCoordsIndex]);
			RestLocations[mapCoordsIndex] = (anObj->Parent->GetRestValue(x, y) != RestType::Empty ? RestType::Empty : RestLocations[mapCoordsIndex]);
		}
	}
}

bool CCoreTilemap::IsHovering(glm::ivec2 aLoc)
{
	MonitorInfo info = MM.GetMonitorDetail(ParentMonitor);
	return HELPER::InBounds(aLoc, info.BoundryNTB);
}

void CCoreTilemap::DestroyTileTemplates()
{
	std::for_each(TilesTemplates.begin(), TilesTemplates.end(), [](TileTemplate* tilesTemplate) { delete tilesTemplate; });
}

void CCoreTilemap::GenerateTileTemplates()
{
	// Desk:
	// --------------------------------------------------------------------------------------
	bool* tableMask = new bool[8]{
		true,	true,	true,	true,
		false,	false,	false,	false, };

	RestType* tableRest = new RestType[8]{
		RestType::Sitting,	RestType::Sitting,	RestType::Sitting,	RestType::Sitting,
		RestType::Empty,	RestType::Empty,	RestType::Empty,	RestType::Empty, };
	// --------------------------------------------------------------------------------------

	// Monitor:
	// --------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------

	TilesTemplates.push_back(new TileTemplate("Desk", glm::ivec4(0, 0, 64, 32), glm::ivec2(0, 0), tableMask, tableRest, glm::ivec2(4, 2), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Monitor", glm::ivec4(64, 0, 20, 17), glm::ivec2(-2, -1), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Rubbish Bin", glm::ivec4(102, 0, 16, 24), glm::ivec2(0, -8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Mug", glm::ivec4(74, 17, 9, 8), glm::ivec2(4, 8), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Trophy", glm::ivec4(64, 17, 10, 12), glm::ivec2(3, 4), new bool[1]{ true }, new RestType[1]{ RestType::Empty }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
	TilesTemplates.push_back(new TileTemplate("Chair", glm::ivec4(84, 0, 18, 34), glm::ivec2(-1, -18), new bool[1]{ true }, new RestType[1]{ RestType::Sitting }, glm::ivec2(1, 1), glm::ivec2(0, 0)));
}

TileTemplate::TileTemplate(std::string aName, glm::ivec4 texLocSize, glm::ivec2 texOffset, bool* aMask, RestType* allRestLocations, glm::ivec2 maskSize, glm::ivec2 maskOffset)
{
	Name = aName;

	TexLocSize = texLocSize;
	TexOffset = texOffset;

	Mask = aMask;
	RestLocations = allRestLocations;
	MaskSize = maskSize;
	MaskOffset = maskOffset;
}

TileTemplate::TileTemplate(std::string filename)
{
}

TileTemplate::~TileTemplate()
{
	delete[] Mask;
	Mask = nullptr;

	delete[] RestLocations;
	RestLocations = nullptr;
}

bool TileTemplate::GetMaskValue(int x, int y)
{
	return Mask[y * MaskSize.x + x];
}

RestType TileTemplate::GetRestValue(int x, int y)
{
	return RestLocations[y * MaskSize.x + x];
}

void TileTemplate::Save(std::string filename)
{
}

void TileTemplate::Load(std::string filename)
{
}
