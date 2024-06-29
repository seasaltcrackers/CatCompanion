#pragma once
#include "mesh.h"

class CTerrain :
	public CMesh
{
public:
	CTerrain();
	CTerrain(int width, int depth, float cellSpacing, float heightScale = 1.0f, float heightOffset = 0.0f);
	CTerrain(std::string filename, int width, int depth, float cellSpacing, float heightScale = 1.0f, float heightOffset = 0.0f);

	float GetHeight(int x, int z);
	float GetHeight(float x, float z);

	int GetGridWidth();
	int GetGridDepth();

	float GetCellSpacing();

private:
	void GenerateMeshData();
	void Smooth();
	float Average(int x, int z);
	bool InBounds(int x, int z);

	std::vector<float> HeightMap;

	float HeightScale;
	float HeightOffset;
	float CellSpacing;

	int GridWidth;
	int GridDepth;

	float LocalWidth;
	float LocalDepth;
};

