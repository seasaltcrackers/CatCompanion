#pragma once
#include <glew.h>
#include <string>
#include <chrono>
#include <vector>
#include <Windows.h>

#include "glm.hpp"

#undef max
#undef min

# define PI 3.14159265358979323846

# define IfThenReturn(x, y) if (x) { return y; }
# define IfThenStatement(x, y) if (x) { y; }


enum class VAlign
{
	Top,
	Center,
	Bottom,
};

enum class HAlign
{
	Left,
	Center,
	Right,
};


namespace HELPER
{
	// String functions
	void SplitString(const std::string& str, std::vector<std::string>& cont, char delim = ' ');
	bool HasEnding(std::string const& fullString, std::string const& ending);

	// Debug functions
	std::string ToStringVec2(glm::vec2 aVec);
	std::string ToStringVec3(glm::vec3 aVec);
	std::string ToStringVec4(glm::vec4 aVec);
	void SetCursorLocation(int x, int y);

	bool IsFloat(std::string string);

	std::string LTrim(const std::string& s);
	std::string RTrim(const std::string& s);
	std::string Trim(const std::string& s);

	void PrintToOutput(std::string s);

	// Vector math
	float LengthDirRadX(float length, float radians);
	float LengthDirRadY(float length, float radians);

	float LengthDirDegX(float length, float degrees);
	float LengthDirDegY(float length, float degrees);

	glm::vec2 LengthDirDeg(float length, float degrees);
	glm::vec2 LengthDirRad(float length, float radians);

	float PointDistance(float x1, float y1, float x2, float y2);
	float PointDistance(glm::vec2 loc1, glm::vec2 loc2);

	float PointDirectionDeg(float x1, float y1, float x2, float y2);
	float PointDirectionDeg(glm::vec2 loc1, glm::vec2 loc2);

	float PointDirectionRad(float x1, float y1, float x2, float y2);
	float PointDirectionRad(glm::vec2 loc1, glm::vec2 loc2);

	glm::vec2 Limit(glm::vec2 loc, float mag);
	glm::vec3 Limit(glm::vec3 loc, float mag);
	glm::vec2 SetMag(glm::vec2 loc, float mag);
	glm::vec3 SetMag(glm::vec3 loc, float mag);

	void AddFrictionForce(glm::vec2& velocity, float friction);
	void AddFrictionForce(glm::vec3& velocity, float friction);

	glm::vec2 RandomVecInRadius(float radius);
	glm::vec3 RandomVecInRadius3D(float radius);

	bool InBounds(int x, int y, int left, int top, int right, int bot);
	bool InBounds(glm::ivec2 loc, int left, int top, int right, int bot);
	bool InBounds(glm::ivec2 loc, glm::ivec2 topLeft, glm::ivec2 botRight);
	bool InBounds(glm::ivec2 loc, glm::vec2 topLeft, glm::vec2 botRight);
	bool InBounds(glm::ivec2 loc, glm::ivec4 boundry);
	bool InBounds(glm::ivec2 loc, RECT boundry);

	// Utility
	int Sign(float aVal);
	float RoundToPlaces(float aVal, int numOfPlaces);
	glm::vec2 RoundToPlaces(glm::vec2 aVal, int numOfPlaces);
	glm::vec3 RoundToPlaces(glm::vec3 aVal, int numOfPlaces);
	int Random(int maxNum);
	int Random(int minNum, int maxNum);
	float Random(float minNum, float maxNum);
	float Random();

	glm::vec3 HSVtoRGB(int H, double S, double V);
	glm::vec3 RGBtoHSV(int r, int g, int b);
	std::string RGBtoHEX(int r, int g, int b);
	glm::vec3 HEXtoRGB(std::string hex);

	float LerpAngleDeg(float a, float b, float t);
	template <typename Fp>
	constexpr Fp Lerp(Fp a, Fp b, Fp t)
	{
		if (a <= 0 && b >= 0 || a >= 0 && b <= 0)
		{
			return t * b + (1 - t) * a;
		}

		if (t == 1)
		{
			return b; // exact
		}

		// Exact at t = 0, monotonic except near t = 1,
		// bounded, determinate, and consistent:
		const Fp x = a + t * (b - a);
		return t > 1 == b > a
			? (b < x ? x : b)
			: (b > x ? x : b); // monotonic near t=1
	}
}
