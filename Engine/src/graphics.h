#pragma once
#include <glew.h>
#include <glm.hpp>
#include <string>
#include <vector>

class CTexture;
class CFont;

namespace GFX
{
	void Initialise();

	void DrawImage(GLuint aTexture, glm::mat4& pvm);

	void DrawSubImage(GLuint aTexture, glm::mat4& pvm, glm::ivec4& locSize);
	void DrawSubImage(GLuint aTexture, glm::mat4& pvm, glm::ivec2& location, glm::ivec2& size);
	void DrawSubImage(GLuint aTexture, glm::mat4& pvm, int x, int y, int width, int height);

	void DrawRectangle(glm::mat4& pv, glm::vec4 colour, int x, int y, int width, int height);
	void DrawRectangle(glm::mat4& pv, glm::vec3 colour, int x, int y, int width, int height);
	void DrawRectangle(glm::mat4& pv, glm::vec4 colour, glm::vec2 aLoc, glm::vec2 aScale);
	void DrawRectangle(glm::mat4& pv, glm::vec3 colour, glm::vec2 aLoc, glm::vec2 aScale);

	void DrawLine(glm::mat4& pv, glm::vec3 colour, glm::vec2 p1, glm::vec2 p2);
	void DrawLine(glm::mat4& pv, glm::vec3 colour, glm::vec2 p1, glm::vec2 p2, float width);
	void DrawLine(glm::mat4& pv, glm::vec4 colour, glm::vec2 p1, glm::vec2 p2);
	void DrawLine(glm::mat4& pv, glm::vec4 colour, glm::vec2 p1, glm::vec2 p2, float width);

	void DrawPoints(glm::mat4& pv, glm::vec4 colour, std::vector<glm::vec2> points, float width);
	void DrawPolygon(glm::mat4& pv, glm::vec4 colour, std::vector<glm::vec2> points, float width);

	void DrawString(glm::mat4 pv, CFont* font, std::string text, glm::ivec2 location, glm::vec3 colour = glm::vec3(), float scale = 1.0f);
	void DrawString(glm::mat4 pv, CFont* font, std::string text, glm::ivec2 location, glm::vec4 colour, float scale = 1.0f);

	glm::mat4 CalculateModelMatrix(glm::vec2 translation, glm::vec2 scale);
	glm::mat4 CalculateModelMatrix(glm::vec2 translation, glm::vec2 scale, glm::vec2 pivot);
	glm::mat4 CalculateModelMatrix(glm::vec2 translation, glm::vec2 scale, float rotationZ);
	glm::mat4 CalculateModelMatrix(glm::vec3 translation, glm::vec3 scale);
	glm::mat4 CalculateModelMatrix(glm::vec3 translation, glm::vec3 scale, float rotationZ);
	glm::mat4 CalculateModelMatrix(glm::vec3 translation, glm::vec3 scale, float rotationX, float rotationY, float rotationZ);

}