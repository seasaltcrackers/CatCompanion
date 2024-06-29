#include <glew.h>
#include <math.h>
#include <chrono>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "helper.h"
#include "timer.h"

/*
	Splits a string by a character and puts it into the given container

	@param str The string to be split
	@param cont The container to store the split strings
	@param delim The split character
*/
void HELPER::SplitString(const std::string& str, std::vector<std::string>& cont, char delim)
{
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim))
	{
		cont.push_back(token);
	}
}

/*
	Checks if a string ends in another string

	@param fullString The string to check the ending of
	@param ending The ending to check for
	@return Returns true if the fullString ends in the ending string
*/
bool HELPER::HasEnding(std::string const& fullString, std::string const& ending)
{
	if (fullString.length() >= ending.length())
	{
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else
	{
		return false;
	}
}

std::string HELPER::ToStringVec2(glm::vec2 aVec)
{
	return "{ " + std::to_string(aVec.x) + " , " + std::to_string(aVec.y) + " }";
}

std::string HELPER::ToStringVec3(glm::vec3 aVec)
{
	return "{ " + std::to_string(aVec.x) + " , " + std::to_string(aVec.y) + " , " + std::to_string(aVec.z) + " }";
}

std::string HELPER::ToStringVec4(glm::vec4 aVec)
{
	return "{ " + std::to_string(aVec.x) + " , " + std::to_string(aVec.y) + " , " + std::to_string(aVec.z) + " , " + std::to_string(aVec.w) + " }";
}

/*
	Gets the X value if you pointed in an (angle) and walked (length) pixels forward

	@param length The distance of the vector
	@param angle The angle in radians
	@return X value
*/
float HELPER::LengthDirRadX(float length, float angle)
{
	return length * cos(angle);
}

/*
	Gets the Y value if you pointed in an (angle) and walked (length) pixels forward

	@param length The distance of the vector
	@param angle The angle in radians
	@return Y value
*/
float HELPER::LengthDirRadY(float length, float angle)
{
	return length * sin(angle);
}

/*
	Gets the X value if you pointed in an (angle) and walked (length) pixels forward

	@param length The distance of the vector
	@param angle The angle in degrees
	@return X value
*/
float HELPER::LengthDirDegX(float length, float angle)
{
	return length * cos(glm::radians(angle));
}

/*
	Gets the Y value if you pointed in an (angle) and walked (length) pixels forward

	@param length The distance of the vector
	@param angle The angle in degrees
	@return Y value
*/
float HELPER::LengthDirDegY(float length, float angle)
{
	return length * sin(glm::radians(angle));
}

glm::vec2 HELPER::LengthDirDeg(float length, float angle)
{
	return glm::vec2(LengthDirDegX(length, angle), LengthDirDegY(length, angle));
}

glm::vec2 HELPER::LengthDirRad(float length, float angle)
{
	return glm::vec2(LengthDirRadX(length, angle), LengthDirRadY(length, angle));
}

/*
	Gets the distance between two points

	@param x1 First point x
	@param y1 First point y
	@param x2 Second point x
	@param y2 Second point y
	@return Distance between the first and second point
*/
float HELPER::PointDistance(float x1, float y1, float x2, float y2)
{
	return sqrtf(powf(x2 - x1, 2.0f) + powf(y2 - y1, 2.0f));
}

float HELPER::PointDistance(glm::vec2 loc1, glm::vec2 loc2)
{
	return sqrtf(powf(loc2.x - loc1.x, 2.0f) + powf(loc2.y - loc1.y, 2.0f));
}


/*
	Gets the direction of one point in the relation to another

	@param x1 First point x
	@param y1 First point y
	@param x2 Second point x
	@param y2 Second point y
	@return Angle in degrees of the second points angle in relation to the first point
*/
float HELPER::PointDirectionDeg(float x1, float y1, float x2, float y2)
{
	return glm::degrees(atan2f(y2 - y1, x2 - x1));
}

float HELPER::PointDirectionDeg(glm::vec2 loc1, glm::vec2 loc2)
{
	return glm::degrees(atan2f(loc2.y - loc1.y, loc2.x - loc1.x));
}

/*
	Gets the direction of one point in the relation to another

	@param x1 First point x
	@param y1 First point y
	@param x2 Second point x
	@param y2 Second point y
	@return Angle in radians of the second points angle in relation to the first point
*/
float HELPER::PointDirectionRad(float x1, float y1, float x2, float y2)
{
	return atan2f(y2 - y1, x2 - x1);
}

float HELPER::PointDirectionRad(glm::vec2 loc1, glm::vec2 loc2)
{
	return atan2f(loc2.y - loc1.y, loc2.x - loc1.x);
}

glm::vec2 HELPER::Limit(glm::vec2 loc, float mag)
{
	if (glm::length(loc) > mag)
	{
		return SetMag(loc, mag);
	}
	else
	{
		return loc;
	}
}

glm::vec3 HELPER::Limit(glm::vec3 loc, float mag)
{
	if (glm::length(loc) > mag)
	{
		return SetMag(loc, mag);
	}
	else
	{
		return loc;
	}
}

glm::vec2 HELPER::SetMag(glm::vec2 loc, float mag)
{
	glm::vec2 returnVal = glm::normalize(loc) * mag;

	IfThenStatement(std::isnan(returnVal.x), returnVal.x = 0);
	IfThenStatement(std::isnan(returnVal.y), returnVal.y = 0);

	return returnVal;
}

glm::vec3 HELPER::SetMag(glm::vec3 loc, float mag)
{
	glm::vec3 returnVal = glm::normalize(loc) * mag;

	IfThenStatement(std::isnan(returnVal.x), returnVal.x = 0);
	IfThenStatement(std::isnan(returnVal.y), returnVal.y = 0);
	IfThenStatement(std::isnan(returnVal.z), returnVal.z = 0);

	return returnVal;
}

void HELPER::AddFrictionForce(glm::vec2& velocity, float friction)
{
	glm::vec2 abs = glm::abs(velocity);
	velocity = glm::sign(velocity) * glm::max(abs - glm::normalize(abs) * TIMER::GetDeltaSeconds() * friction, glm::vec2());
}

void HELPER::AddFrictionForce(glm::vec3& velocity, float friction)
{
	glm::vec3 abs = glm::abs(velocity);
	velocity = glm::sign(velocity) * glm::max(abs - glm::normalize(abs) * TIMER::GetDeltaSeconds() * friction, glm::vec3());
}

glm::vec2 HELPER::RandomVecInRadius(float radius)
{
	float angle = Random() * (float)PI * 2.0f;
	float length = sqrtf(Random()) * radius;
	return LengthDirRad(length, angle);
}

glm::vec3 HELPER::RandomVecInRadius3D(float radius)
{
	glm::vec3 returnVec;

	if (radius > 0)
	{
		do
		{
			returnVec.x = (Random() * radius * 2.0f) - radius;
			returnVec.y = (Random() * radius * 2.0f) - radius;
			returnVec.z = (Random() * radius * 2.0f) - radius;

		} while (glm::length(returnVec) > radius);
	}

	return returnVec;

}

bool HELPER::InBounds(int x, int y, int left, int top, int right, int bot)
{
	return (x >= left &&
			y >= top &&
			x < right &&
			y < bot);
}

bool HELPER::InBounds(glm::ivec2 loc, int left, int top, int right, int bot)
{
	return (loc.x >= left &&
			loc.y >= top &&
			loc.x < right &&
			loc.y < bot);
}

bool HELPER::InBounds(glm::ivec2 loc, glm::ivec2 topLeft, glm::ivec2 botRight)
{
	return (loc.x >= topLeft.x &&
			loc.y >= topLeft.y &&
			loc.x < botRight.x &&
			loc.y < botRight.y);
}

bool HELPER::InBounds(glm::ivec2 loc, glm::vec2 topLeft, glm::vec2 botRight)
{
	return (loc.x >= topLeft.x &&
			loc.y >= topLeft.y &&
			loc.x < botRight.x &&
			loc.y < botRight.y);
}

bool HELPER::InBounds(glm::ivec2 loc, glm::ivec4 boundry)
{
	return (loc.x >= boundry.x &&
			loc.y >= boundry.y &&
			loc.x < boundry.z &&
			loc.y < boundry.w);
}

bool HELPER::InBounds(glm::ivec2 loc, RECT boundry)
{
	return (loc.x >= boundry.left &&
			loc.y >= boundry.top &&
			loc.x < boundry.right &&
			loc.y < boundry.bottom);
}

int HELPER::Sign(float aVal)
{
	return aVal == 0.0f ? 0.0f : (int)(aVal / std::abs(aVal));
}

float HELPER::RoundToPlaces(float aVal, int numOfPlaces)
{
	int multiplier = (int)std::pow(10, numOfPlaces);
	return std::round(aVal * multiplier) / multiplier;
}

glm::vec2 HELPER::RoundToPlaces(glm::vec2 aVal, int numOfPlaces)
{
	return glm::vec2(HELPER::RoundToPlaces(aVal.x, numOfPlaces), HELPER::RoundToPlaces(aVal.y, numOfPlaces));
}

glm::vec3 HELPER::RoundToPlaces(glm::vec3 aVal, int numOfPlaces)
{
	return glm::vec3(HELPER::RoundToPlaces(aVal.x, numOfPlaces), HELPER::RoundToPlaces(aVal.y, numOfPlaces), HELPER::RoundToPlaces(aVal.z, numOfPlaces));
}

void HELPER::SetCursorLocation(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

bool HELPER::IsFloat(std::string string) 
{
	std::istringstream iss(Trim(string));
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
	// Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

std::string HELPER::LTrim(const std::string& s)
{
	const std::string WHITESPACE = " \n\r\t\f\v";

	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string HELPER::RTrim(const std::string& s)
{
	const std::string WHITESPACE = " \n\r\t\f\v";

	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string HELPER::Trim(const std::string& s)
{
	return RTrim(LTrim(s));
}

void HELPER::PrintToOutput(std::string s)
{
	s += '\n';
	OutputDebugStringA(s.c_str());
}


/*
	Generates a random number inbetween 0 and a number exclusive

	@param maxNum Maximum random number
	@return A random integer inbetween 0 and maxNum
*/
int HELPER::Random(int maxNum)
{
	return rand() % maxNum;
}

/*
	Generates a random number inbetween A and B exclusive

	@param minNum Minimum random number
	@param maxNum Maximum random number
	@return A random integer inbetween minNum and maxNum
*/
int HELPER::Random(int minNum, int maxNum)
{
	return (rand() % (maxNum - minNum)) + minNum;
}

float HELPER::Random(float minNum, float maxNum)
{
	return HELPER::Random() * (maxNum - minNum) + minNum;
}

float HELPER::Random()
{
	return (float)rand() / (float)RAND_MAX;
}

/*
 * H(Hue): 0 - 360 degree (integer)
 * S(Saturation): 0 - 1.00 (double)
 * V(Value): 0 - 1.00 (double)
 *
 * output[3]: Output, array size 3, int
 */
glm::vec3 HELPER::HSVtoRGB(int H, double S, double V)
{
	glm::vec3 output;

	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	output.x = (float)((Rs + m) * 255.0);
	output.y = (float)((Gs + m) * 255.0);
	output.z = (float)((Bs + m) * 255.0);

	return output;
}

glm::vec3 HELPER::RGBtoHSV(int src_r, int src_g, int src_b)
{
	double r = src_r / 255.0;
	double g = src_g / 255.0;
	double b = src_b / 255.0;

	glm::vec3 out;
	double min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;

	max = r > g ? r : g;
	max = max > b ? max : b;

	out.z = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.y = 0;
		out.x = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.y = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		out.y = 0.0;
		out.x = NAN;                            // its now undefined
		return out;
	}
	if (r >= max)                           // > is bogus, just keeps compilor happy
		out.x = (g - b) / delta;        // between yellow & magenta
	else
		if (g >= max)
			out.x = 2.0 + (b - r) / delta;  // between cyan & yellow
		else
			out.x = 4.0 + (r - g) / delta;  // between magenta & cyan

	out.x *= 60.0;                              // degrees

	if (out.x < 0.0)
		out.x += 360.0;

	return out;
}

std::string HELPER::RGBtoHEX(int r, int g, int b)
{
	char hexColor[8];
	std::snprintf(hexColor, sizeof hexColor, "#%02x%02x%02x", r, g, b);
	return hexColor;
}

glm::vec3 HELPER::HEXtoRGB(std::string hex)
{
	// Get rid of # at the start
	hex = hex.substr(1, hex.size() - 1);

	int r, g, b;
	if (sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b) == 3)
	{
		return glm::vec3(r, g, b);
	}
	else
	{
		throw "Hex not valid value";
	}
}

float HELPER::LerpAngleDeg(float a, float b, float t)
{
	float dA = std::abs(a - b);
	float dB = std::abs((a + 360) - b);
	float dC = std::abs(a - (b + 360));

	if (dA <= dB && dA <= dC)
	{
		return Lerp(a, b, t);
	}
	else if (dB < dA)
	{
		return std::fmodf(Lerp(a + 360, b, t), 360.0f);
	}
	else
	{
		return std::fmodf(Lerp(a, b + 360, t), 360.0f);
	}
}
