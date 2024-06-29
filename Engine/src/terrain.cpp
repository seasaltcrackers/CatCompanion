#include <glm.hpp>
#include <random>
#include <fstream>
#include <SOIL.h>

#include "perlinNoise.h"
#include "terrain.h"
#include "helper.h"

CTerrain::CTerrain() :
	CMesh()
{
	GridWidth = 0;
	GridDepth = 0;

	CellSpacing = 1.0f;
	HeightScale = 1.0f;
	HeightOffset = 0.0f;
}

CTerrain::CTerrain(int width, int depth, float cellSpacing, float heightScale, float heightOffset) :
	CMesh()
{
	GridWidth = width;
	GridDepth = depth;

	CellSpacing = cellSpacing;
	HeightScale = heightScale;
	HeightOffset = heightOffset;

	std::vector<std::vector<double>> grid = CPerlinNoise::LayeredNoise(width, depth, 5, 2.0, 0.5);
	HeightMap.resize(GridWidth * GridDepth);

	for (int x = 0; x < width; ++x)
	{
		for (int z = 0; z < depth; ++z)
		{
			HeightMap[z * GridWidth + x] = (float)grid[x][z] * heightScale + heightOffset;
		}
	}

	GenerateMeshData();
}

CTerrain::CTerrain(std::string filename, int width, int depth, float cellSpacing, float heightScale, float heightOffset) :
	CMesh()
{
	GridWidth = width;
	GridDepth = depth;

	CellSpacing = cellSpacing;
	HeightScale = heightScale;
	HeightOffset = heightOffset;

	LocalWidth = (width - 1) * cellSpacing;
	LocalDepth = (depth - 1) * cellSpacing;

	const size_t dataSize = GridWidth * GridDepth;

	// A height for each vertex
	std::vector<unsigned char> in(dataSize);

	// Open the file.
	std::ifstream inFile;
	inFile.open(filename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	HeightMap.resize(dataSize);
	for (unsigned int i = 0; i < dataSize; ++i)
	{
		HeightMap[i] = (float)in[i] * heightScale + heightOffset;
	}

	Smooth();
	GenerateMeshData();
}


float CTerrain::GetHeight(int x, int z)
{
	return HeightMap[z * GridWidth + x];
}

float CTerrain::GetHeight(float x, float z)
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f * LocalWidth) / CellSpacing;
	float d = (z + 0.5f * LocalDepth) / CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	IfThenReturn(!InBounds(col, row), 0.0f);
	IfThenReturn(!InBounds(col + 1, row + 1), 0.0f);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = HeightMap[row * GridWidth + col];
	float B = HeightMap[row * GridWidth + col + 1];
	float C = HeightMap[(row + 1) * GridWidth + col];
	float D = HeightMap[(row + 1) * GridWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s) * uy + (1.0f - t) * vy;
	}
}

int CTerrain::GetGridWidth()
{
	return GridWidth;
}

int CTerrain::GetGridDepth()
{
	return GridDepth;
}

float CTerrain::GetCellSpacing()
{
	return CellSpacing;
}

void CTerrain::GenerateMeshData()
{
	std::vector<GLfloat> tempVerts;

	const float xOffset = -GridWidth * CellSpacing * 0.5f;
	const float zOffset = -GridDepth * CellSpacing * 0.5f;

	float xLoc = 0.0f;
	float zLoc = 0.0f;

	float xUV = 0.0f;
	float yUV = 0.0f;

	for (int x = 0; x < GridWidth; ++x)
	{
		xUV = (x / (float)GridWidth);
		xLoc = xUV * GridWidth * CellSpacing + xOffset;

		for (int z = 0; z < GridDepth; ++z)
		{
			const int index = z * GridWidth + x;

			yUV = (z / (float)GridDepth);
			zLoc = yUV * GridDepth * CellSpacing + zOffset;

			float T = GetHeight(x, std::max(0, z - 1));
			float R = GetHeight(std::min(x + 1, GridWidth - 1), z);
			float B = GetHeight(x, std::min(GridDepth - 1, z + 1));
			float L = GetHeight(std::max(x - 1, 0), z);

			glm::vec3 normal = {
				-2.0f * (R - L),
				4.0f,
				-2.0f * (B - T) };

			normal = glm::normalize(normal);

			tempVerts.push_back(xLoc);				// position x
			tempVerts.push_back(GetHeight(x, z));	// position y
			tempVerts.push_back(zLoc);				// position z

			tempVerts.push_back(normal.x);			// normal x
			tempVerts.push_back(normal.y);			// normal y
			tempVerts.push_back(normal.z);			// normal z

			tempVerts.push_back(xUV);				// UV x
			tempVerts.push_back(yUV);				// UV y
		}
	}

	for (int z = 0; z < GridDepth - 1; ++z)
	{
		for (int x = 0; x < GridWidth - 1; ++x)
		{
			Indicies.push_back(z * GridWidth + x);
			Indicies.push_back(z * GridWidth + x + 1);
			Indicies.push_back((z + 1) * GridWidth + x);

			Indicies.push_back((z + 1) * GridWidth + x);
			Indicies.push_back(z * GridWidth + x + 1);
			Indicies.push_back((z + 1) * GridWidth + x + 1);
		}
	}

	VAO = GeneratePNTVAO(tempVerts, Indicies);
	IndiceCount = (int)Indicies.size();
}

void CTerrain::Smooth()
{
	std::vector<float> dest(HeightMap.size());

	for (int z = 0; z < GridDepth; ++z)
	{
		for (int x = 0; x < GridWidth; ++x)
		{
			dest[z * GridWidth + x] = Average(x, z);
		}
	}

	// Replace the old heightmap with the filtered one.
	HeightMap = dest;
}

float CTerrain::Average(int x, int z)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for (int m = z - 1; m <= z + 1; ++m)
	{
		for (int n = x - 1; n <= x + 1; ++n)
		{
			if (InBounds(n, m))
			{
				avg += HeightMap[m * GridWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

bool CTerrain::InBounds(int x, int z)
{
	// True if ij are valid indices; false otherwise.
	return
		x >= 0 && x < GridWidth &&
		z >= 0 && z < GridDepth;
}
