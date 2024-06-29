#pragma once
#include <vector>

#include "texture.h"
#include "layer.h"

class CMesh;
class CProgram;

struct TilemapTexture
{
	CTexture* TilesTexture;
	int SplitX;
	int SplitY;

	TilemapTexture(CTexture* aTexture, int aSplitX, int aSplitY);
	~TilemapTexture();

	int TileWidth;
	int TileHeight;

	int GetID(int x, int y);
	int GetLocationX(int ID);
	int GetLocationY(int ID);
};

struct Tile
{
	virtual int GetID() = 0;
};

struct StaticTile : Tile
{
	int ID;

	StaticTile(int id);
	int GetID() override;
};

struct AnimatedTile : Tile
{
	std::vector<int> IDs;

	AnimatedTile(std::vector<int> ids);
	int GetID() override;
};

struct TilemapGrid
{
	int Width;
	int Height;

	Tile** Grid;

	TilemapGrid(int aWidth, int aHeight);
	TilemapGrid(Tile** aGrid, int aWidth, int aHeight);
	~TilemapGrid();
	int GetGridVal(int x, int y);
	void SetGridVal(int x, int y, int aVal);
};

class CTilemapLayer :
	public ILayer
{
public:
	CTilemapLayer(TilemapTexture* aTileSet, TilemapGrid* aTileGrid);
	~CTilemapLayer();

	static CTilemapLayer* Load(std::string filename);
	void Save(std::string filename);

	void Render() override;
	void Update() override;

	int GetCell(int x, int y);
	int GetCell(glm::ivec2 loc);

	void SetCell(int x, int y, int cellID);
	void SetCell(glm::ivec2 loc, int cellID);

	int GetCellFromPixel(int x, int y);
	int GetCellFromPixel(glm::vec2 loc);

	void SetCellFromPixel(int x, int y, int cellID);
	void SetCellFromPixel(glm::vec2 loc, int cellID);

	int GetCellXFromPixel(int x);
	int GetCellYFromPixel(int y);

	glm::vec2 CellToPixelCoords(glm::ivec2 aLoc);
	glm::ivec2 PixelToCellCoords(glm::vec2 aLoc);

	int GetTileWidth();
	int GetTileHeight();
	glm::vec2 GetTileSize();

	int GetGridWidth();
	int GetGridHeight();
	glm::vec2 GetGridSize();

	static void CleanUp();
	static CTilemapLayer* GetTilemap(std::string tilemapName);
	static void InsertTilemap(std::string aName, CTilemapLayer* aTilemap);

private:
	CProgram* SpriteSheetProgram;
	CMesh* TileMesh;

	TilemapTexture* TileSet;
	TilemapGrid* TileGrid;

	static std::map<std::string, CTilemapLayer*> Tilemaps;
};