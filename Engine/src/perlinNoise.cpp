#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

#include "texture.h"
#include "perlinNoise.h"
#include <iostream>

CPerlinNoise::CPerlinNoise()
{
	// Initialize the permutation vector with the reference values
	p = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

CPerlinNoise::CPerlinNoise(unsigned int seed)
{
	p.resize(256);

	// Fill p with values from 0 to 255
	std::iota(p.begin(), p.end(), 0);

	// Initialize a random engine with seed
	std::default_random_engine engine(seed);

	// Suffle  using the above random engine
	std::shuffle(p.begin(), p.end(), engine);

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

double CPerlinNoise::Noise(double x, double y, double z)
{
	// Find the unit cube that contains the point
	int X = (int)std::floor(x) & 255;
	int Y = (int)std::floor(y) & 255;
	int Z = (int)std::floor(z) & 255;

	// Find relative x, y,z of point in cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	// Compute fade curves for each of x, y, z
	double u = Fade(x);
	double v = Fade(y);
	double w = Fade(z);

	// Hash coordinates of the 8 cube corners
	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	// Add blended results from 8 corners of cube
	double res = Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z), Grad(p[BA], x - 1, y, z)), Lerp(u, Grad(p[AB], x, y - 1, z), Grad(p[BB], x - 1, y - 1, z))), Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1), Grad(p[BA + 1], x - 1, y, z - 1)), Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1), Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	return (res + 1.0) / 2.0;
}

std::vector<std::vector<double>> CPerlinNoise::LayeredNoise(int width, int height, int layers, double lacunarity, double persistance)
{
	std::vector<std::vector<double>> matrix(width);

	for (int i = 0; i < width; ++i)
	{
		matrix[i] = std::vector<double>(height);
		std::fill(matrix[i].begin(), matrix[i].end(), 0.0);
	}

	for (int i = 0; i < layers; ++i)
	{
		CPerlinNoise tempNoise(rand());

		double freq = std::pow(lacunarity, i);
		double frequencyX = freq / width;
		double frequencyY = freq / height;

		double amplitude = std::pow(persistance, i);
		double zOffset = -amplitude * 0.5f;

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				matrix[x][y] += tempNoise.Noise(((double)x * frequencyX), ((double)y * frequencyY), 1.0) * amplitude + zOffset;
			}
		}
	}

	return matrix;
}

GLuint* CPerlinNoise::GetLayeredNoise(const LayeredNoiseData& variables)
{
	//GLuint ID = 0;
	GLuint* data = new GLuint[variables.Width * variables.Height];
	
	for (unsigned int i = 0; i < variables.Width * variables.Height; ++i)
	{
		data[i] = 0;
	}

	for (int i = 0; i < variables.Layers; ++i)
	{
		double freq = std::pow(variables.Lacunarity, i);
		double frequencyX = freq / variables.Width;
		double frequencyY = freq / variables.Height;

		double amplitude = std::pow(variables.Persistance, i);

		for (unsigned int x = 0; x < variables.Width; ++x)
		{
			for (unsigned int y = 0; y < variables.Height; ++y)
			{
				data[y * variables.Width + x] += (int)(Noise((((double)x + variables.OffsetX) * frequencyX), (((double)y + variables.OffsetY) * frequencyY), 1.0) * amplitude * 100.0f);
			}
		}
	}

	return data;
}

float CPerlinNoise::GetHeightFromLayeredNoise(const LayeredNoiseData& data, float x, float y)
{	
	float height = 0.0f;

	for (int i = 0; i < data.Layers; ++i)
	{
		double freq = std::pow(data.Lacunarity, i);
		double frequencyX = freq / data.Width;
		double frequencyY = freq / data.Height;

		double amplitude = std::pow(data.Persistance, i);

		height += (int)(Noise((((double)x + data.OffsetX) * frequencyX), (((double)y + data.OffsetY) * frequencyY), 1.0) * amplitude * 100.0f);
	}

	return height;
}

double CPerlinNoise::Fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double CPerlinNoise::Lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double CPerlinNoise::Grad(int hash, double x, double y, double z)
{
	int h = hash & 15;

	// Convert lower 4 bits of hash into 12 gradient directions
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
