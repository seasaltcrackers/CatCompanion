#pragma once
#include <string>
#include <glm.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H


struct FontChar
{
	GLuint TextureID = 0;	// Texture ID
	glm::ivec2 Size;	// Size of the Glyph
	glm::ivec2 Bearing;	// Offset of the Glyph (top left) from the baseline
	GLuint Advance = 0;		// How far to move for the next character
};


class CFont
{
public:
	CFont(std::string filename, int size);
	~CFont();

	glm::ivec2 TextBounds(std::string text, float scale);

	static void CleanUp();
	static void GenerateFonts();
	static CFont* GetFont(std::string fontName);
	
	std::map<GLchar, FontChar> Characters;
	int Size;

private:
	GLuint GenerateTexture(FT_Face face);

	static std::string GetFilepath();
	static std::map<std::string, CFont*> Fonts;
};