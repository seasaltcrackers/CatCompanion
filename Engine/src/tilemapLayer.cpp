#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "tilemapLayer.h"
#include "timer.h"
#include "camera.h"
#include "mesh.h"
#include "program.h"
#include "helper.h"

std::map<std::string, CTilemapLayer*> CTilemapLayer::Tilemaps;

TilemapGrid::TilemapGrid(int aWidth, int aHeight)
{
	Width = aWidth;
	Height = aHeight;

	Grid = new Tile * [aWidth * aHeight];

	for (int i = 0; i < aWidth * aHeight; i++)
	{
		Grid[i] = new StaticTile(0);    // Initialize all elements to zero.
	}
}

TilemapGrid::TilemapGrid(Tile** aGrid, int aWidth, int aHeight)
{
	Grid = aGrid;
	Width = aWidth;
	Height = aHeight;
}

TilemapGrid::~TilemapGrid()
{
	for (int i = 0; i < Width * Height; i++)
	{
		delete Grid[i];    // Initialize all elements to zero.
	}
	delete [] Grid;
}

int TilemapGrid::GetGridVal(int x, int y)
{
	return (HELPER::InBounds(x, y, 0, 0, Width, Height) ? Grid[y * Width + x]->GetID() : 0);
}

void TilemapGrid::SetGridVal(int x, int y, int aVal)
{
	delete Grid[y * Width + x];
	Grid[y * Width + x] = new StaticTile(aVal);
}

TilemapTexture::TilemapTexture(CTexture* aTexture, int aSplitX, int aSplitY)
{
	TilesTexture = aTexture;
	SplitX = aSplitX;
	SplitY = aSplitY;

	TileWidth = TilesTexture->GetWidth() / SplitX;
	TileHeight = TilesTexture->GetHeight() / SplitY;
}

TilemapTexture::~TilemapTexture()
{
	delete TilesTexture;
}

int TilemapTexture::GetID(int x, int y)
{
	return y * SplitX + x;
}

int TilemapTexture::GetLocationX(int ID)
{
	return ID % SplitX;
}

int TilemapTexture::GetLocationY(int ID)
{
	return (int)floorf((float)ID / (float)SplitX);
}

CTilemapLayer::CTilemapLayer(TilemapTexture* aTileSet, TilemapGrid* aTileGrid)
{
	LayerType = "CTilemapLayer";

	TileSet = aTileSet;
	TileGrid = aTileGrid;

	TileMesh = CMesh::Generate2DRectangle((float)aTileSet->TileWidth, (float)aTileSet->TileHeight);
	SpriteSheetProgram = CProgram::GetProgram("SpriteSheet");
}

CTilemapLayer::~CTilemapLayer()
{
	delete TileSet;
	delete TileGrid;
}

// Assumes tile id 0 is transparent
void CTilemapLayer::Render()
{
	// Pre-load the view proj matrix
	glm::mat4 viewProjMat = CCamera::GetProjViewMatrix();

	// Bind program
	glUseProgram(SpriteSheetProgram->GetProgramID());

	// Set Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TileSet->TilesTexture->GetID());
	glUniform1i(SpriteSheetProgram->GetUniform("tex"), 0);

	// Bind mesh
	glBindVertexArray(TileMesh->GetVAO());
	int tempIndicieCount = TileMesh->GetIndicieCount();

	GLuint spriteIndexID = SpriteSheetProgram->GetUniform("spriteIndex");
	GLuint pvmID = SpriteSheetProgram->GetUniform("PVM");

	// Set number of splits
	glUniform2i(SpriteSheetProgram->GetUniform("splitNumber"), TileSet->SplitX, TileSet->SplitY);

	for (int x = 0; x < TileGrid->Width; x++)
	{
		int locX = x * TileSet->TileWidth;

		for (int y = 0; y < TileGrid->Height; y++)
		{
			int val = TileGrid->GetGridVal(x, y);

			if (val != 0)
			{
				// Set the matrix
				glm::mat4 pvm = viewProjMat * glm::translate(glm::mat4(), glm::vec3(locX, y * TileSet->TileHeight, 0.0f));
				glUniformMatrix4fv(pvmID, 1, GL_FALSE, glm::value_ptr(pvm));

				// Set the sprite index
				glUniform2i(spriteIndexID, TileSet->GetLocationX(val), TileSet->GetLocationY(val));

				// Draw the tile
				glDrawElements(GL_TRIANGLES, tempIndicieCount, GL_UNSIGNED_INT, 0);
			}
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

void CTilemapLayer::Update()
{
}


CTilemapLayer* CTilemapLayer::Load(std::string filename)
{
	// TilemapTexture
	CTexture* aTexture = nullptr;
	int splitX = 0;
	int splitY = 0;

	// TilemapGrid
	Tile** tiles = nullptr;
	int width = 0;
	int height = 0;

	std::ifstream file(filename); // Open the file
	std::string str;

	// Read through the lines
	while (std::getline(file, str))
	{
		std::vector<std::string> subString1;
		HELPER::SplitString(str, subString1, ':');

		if (subString1[0] == "Tileset")
		{
			aTexture = new CTexture(subString1[1]);
		}
		else if (subString1[0] == "SplitX")
		{
			splitX = stoi(subString1[1]);
		}
		else if (subString1[0] == "SplitY")
		{
			splitY = stoi(subString1[1]);
		}
		else if (subString1[0] == "Width")
		{
			width = stoi(subString1[1]);
		}
		else if (subString1[0] == "Height")
		{
			height = stoi(subString1[1]);
		}
		else
		{
			tiles = new Tile * [width * height];

			int i = 0;

			while (std::getline(file, str))
			{
				// Split the variables and the object type
				std::vector<std::string> subStrings2;
				HELPER::SplitString(str, subStrings2, ',');

				//std::for_each(subStrings2.begin(), subStrings2.end(), [&tiles, &i](std::string str) { tiles[i] = (stoi(str)); ++i; });
				std::vector<std::string>::iterator it = subStrings2.begin();
				while (it != subStrings2.end())
				{
					std::vector<std::string> framesStr;
					HELPER::SplitString(*it, framesStr, ':');

					if (framesStr.size() > 1)
					{
						std::vector<int> framesInt;

						for (int i = 0; i < (int)framesStr.size(); ++i)
						{
							framesInt.push_back(stoi(framesStr[i]));
						}

						tiles[i] = new AnimatedTile(framesInt);
					}
					else
					{
						tiles[(unsigned int)i] = new StaticTile((int)stoi(*it));
					}

					i++;
					it++;
				}
			}

			break;
		}
	}

	file.close();

	return new CTilemapLayer(new TilemapTexture(aTexture, splitX, splitY), new TilemapGrid(tiles, width, height));
}

void CTilemapLayer::Save(std::string filename)
{
	// Select the file to write to
	std::ofstream outfile(filename);

	outfile << "Tileset:" << TileSet->TilesTexture->GetFilepath() << std::endl;
	outfile << "SplitX:" << TileSet->SplitX << std::endl;
	outfile << "SplitY:" << TileSet->SplitY << std::endl;
	outfile << "Width:" << TileGrid->Width << std::endl;
	outfile << "Height:" << TileGrid->Height << std::endl;
	outfile << "TileGrid:" << std::endl;

	for (int x = 0; x < TileGrid->Width; ++x)
	{
		outfile << std::to_string(TileGrid->GetGridVal(x, 0));

		for (int y = 1; y < TileGrid->Height; ++y)
		{
			outfile << "," << std::to_string(TileGrid->GetGridVal(x, y));
		}

		outfile << std::endl;
	}

	outfile.close(); // Close the file
}

int CTilemapLayer::GetCell(int x, int y)
{
	return TileGrid->GetGridVal(x, y);
}

int CTilemapLayer::GetCell(glm::ivec2 loc)
{
	return TileGrid->GetGridVal(loc.x, loc.y);
}

void CTilemapLayer::SetCell(int x, int y, int cellID)
{
	TileGrid->SetGridVal(x, y, cellID);
}

void CTilemapLayer::SetCell(glm::ivec2 loc, int cellID)
{
	TileGrid->SetGridVal(loc.x, loc.y, cellID);
}

int CTilemapLayer::GetCellFromPixel(int x, int y)
{
	return TileGrid->GetGridVal(GetCellXFromPixel(x), GetCellYFromPixel(y));
}

int CTilemapLayer::GetCellFromPixel(glm::vec2 loc)
{
	return TileGrid->GetGridVal(GetCellXFromPixel((int)loc.x), GetCellYFromPixel((int)loc.y));
}

void CTilemapLayer::SetCellFromPixel(int x, int y, int cellID)
{
	TileGrid->SetGridVal(GetCellXFromPixel(x), GetCellYFromPixel(y), cellID);
}

void CTilemapLayer::SetCellFromPixel(glm::vec2 loc, int cellID)
{
	TileGrid->SetGridVal(GetCellXFromPixel((int)loc.x), GetCellYFromPixel((int)loc.y), cellID);
}

int CTilemapLayer::GetCellXFromPixel(int x)
{
	return (int)floorf((float)x / (float)TileSet->TileWidth);
}

int CTilemapLayer::GetCellYFromPixel(int y)
{
	return (int)floorf((float)y / (float)TileSet->TileHeight);
}

glm::vec2 CTilemapLayer::CellToPixelCoords(glm::ivec2 aLoc)
{
	return aLoc * glm::ivec2(TileSet->TileWidth, TileSet->TileHeight);
}

glm::ivec2 CTilemapLayer::PixelToCellCoords(glm::vec2 aLoc)
{
	return glm::ivec2(GetCellXFromPixel((int)aLoc.x), GetCellYFromPixel((int)aLoc.y));
}

int CTilemapLayer::GetTileWidth()
{
	return TileSet->TileWidth;
}

int CTilemapLayer::GetTileHeight()
{
	return TileSet->TileHeight;
}

glm::vec2 CTilemapLayer::GetTileSize()
{
	return glm::vec2(TileSet->TileWidth, TileSet->TileHeight);
}

int CTilemapLayer::GetGridWidth()
{
	return TileGrid->Width;
}

int CTilemapLayer::GetGridHeight()
{
	return TileGrid->Height;
}

glm::vec2 CTilemapLayer::GetGridSize()
{
	return glm::vec2(TileGrid->Width, TileGrid->Height);
}

void CTilemapLayer::CleanUp()
{
	std::for_each(Tilemaps.begin(), Tilemaps.end(), [](std::pair<const std::string, CTilemapLayer*>& it) { delete it.second; });
	Tilemaps.clear();
}

CTilemapLayer* CTilemapLayer::GetTilemap(std::string tilemapName)
{
	auto it = Tilemaps.find(tilemapName);
	if (it != Tilemaps.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "WARNING: Failed to find TILEMAP '" << tilemapName << "'" << std::endl;
		return nullptr;
	}
}

void CTilemapLayer::InsertTilemap(std::string aName, CTilemapLayer* aTilemap)
{
	Tilemaps.insert({ aName, aTilemap });
}

StaticTile::StaticTile(int id)
{
	ID = id;
}

int StaticTile::GetID()
{
	return ID;
}

AnimatedTile::AnimatedTile(std::vector<int> ids)
{
	IDs = ids;
}

int AnimatedTile::GetID()
{
	return IDs[(int)std::floorf(std::fmodf(TIMER::GetTotalSeconds() * 15.0f, (float)IDs.size()))];
}
