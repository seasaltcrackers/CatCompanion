#pragma once
#include <glm.hpp>
#include <string>
#include <vector>
#include <glew.h>

#include "layer.h"
#include "monitorManager.h"

class CCoreTilemap;
class CProgram;
class CMesh;
class CTexture;

const int TileSize = 16;

enum class RestType
{
	Empty,
	Sitting,
	CurledUp,
	Flopped,
};

struct TileTemplate
{
	TileTemplate(std::string aName, glm::ivec4 texLocSize, glm::ivec2 texOffset, bool* aMask, RestType* allRestLocations, glm::ivec2 maskSize, glm::ivec2 maskOffset);
	TileTemplate(std::string filename);
	~TileTemplate();

	std::string Name;

	glm::ivec4 TexLocSize;
	glm::ivec2 TexOffset;

	bool* Mask;
	RestType* RestLocations;
	glm::ivec2 MaskSize;
	glm::ivec2 MaskOffset;

	bool GetMaskValue(int x, int y);
	RestType GetRestValue(int x, int y);

	void Save(std::string filename);
	void Load(std::string filename);

};

struct TileObject
{
	TileObject(CCoreTilemap* parentTilemap, TileTemplate* aTemplate, glm::ivec2 cellLoc);

	CCoreTilemap* ParentTilemap;
	TileTemplate* Parent;
	glm::ivec2 CellLocation;
	glm::mat4 ModelMatrix;

	void SetCellLocation(glm::ivec2& newLocation);
	void UpdateModelMat();
};

class CCoreTilemap :
	public ILayer
{
public:
	CCoreTilemap(int parentMonitor);
	~CCoreTilemap();

	int GetWidth();
	int GetHeight();
	glm::ivec2 GetSize();

	GLuint GetTileTexture();

	glm::ivec2 GetOffset();

	void Render() override;
	void Update() override;

	glm::ivec2 PixelToCell(glm::ivec2 aLocation);
	glm::ivec2 CellToPixel(glm::ivec2 aLocation);

	TileObject* GetObjectFromCell(glm::ivec2 cellLocation);
	TileObject* GetObjectFromPixel(glm::ivec2 pixelLocation);
	
	bool IsObjValid(TileObject* anObj);
	void StampObject(TileObject* anObj);
	void RemoveObject(TileObject* anObj);

	bool IsHovering(glm::ivec2 aLoc);

	static void DestroyTileTemplates();
	static void GenerateTileTemplates();

	static float GetTileScaler();

	static std::vector<TileTemplate*> TilesTemplates;

private:
	int ParentMonitor;
	CMonitorManager& MM;

	TileObject** Tiles; // Used for drawing
	TileObject** Mask; // Pointers to the objects whose masks take up the tile
	RestType* RestLocations; // Holds the rest locations for the cat

	glm::ivec2 Offset; // The offset of the entire grid
	int BaseOffset;
	
	CProgram* TileShader;
	CMesh* TileMesh;
	CTexture* TileTexture;

	int Width;
	int Height;
};

