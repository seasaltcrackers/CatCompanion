#pragma once
#include <vector>
#include <glew.h>

class CTexture;

struct LayeredNoiseData
{
	unsigned int Width = 10;
	unsigned int Height = 10;
	
	int Seed = rand();
	int Layers = 5;

	float OffsetX = 0.0f;
	float OffsetY = 0.0f;

	double Lacunarity = 2.0f;
	double Persistance = 0.5f;
};

// https://github.com/sol-prog/Perlin_Noise
class CPerlinNoise
{
public:
	CPerlinNoise();
	CPerlinNoise(unsigned int seed);

	double Noise(double x, double y, double z);

	static std::vector<std::vector<double>> LayeredNoise(int width, int height, int layers, double lacunarity = 2.0f, double persistance = 0.5f);
	GLuint* GetLayeredNoise(const LayeredNoiseData& data);
	float GetHeightFromLayeredNoise(const LayeredNoiseData& data, float x, float y);

private:
	std::vector<int> p;

	double Fade(double t);
	double Lerp(double t, double a, double b);
	double Grad(int hash, double x, double y, double z);



};

