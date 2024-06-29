#include <glew.h>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "font.h"
#include "helper.h"

std::map<std::string, CFont*> CFont::Fonts;

CFont::CFont(std::string filename, int size)
{
	Size = size;

	FT_Library ft;	// Font Library Object to load the font
	FT_Face face;	// Each character is called a glyph and Face is the collection of glyphs

	IfThenStatement(FT_Init_FreeType(&ft) != 0, std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl);
	IfThenStatement(FT_New_Face(ft, (GetFilepath() + filename).c_str(), 0, &face) != 0, std::cout << "ERROR::FREETYPE: Failed to Load font" << std::endl);

	//FT_Set_Pixel_Sizes(face, 0, 48);		// Set size to load Glyph
	FT_Set_Pixel_Sizes(face, 0, size);		// Set size to load Glyph
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// Disable byte-alignment restriction

	// Load he first 128 characters of ASCII set
	for (GLubyte character = 0; character < 128; character++)
	{
		// Load the character glyph into face
		if (FT_Load_Char(face, character, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
			continue;
		}

		GLuint texture = GenerateTexture(face); // Generate a texture for each char (glyph)

		// Now to store character for later use
		FontChar fontChar = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(GLuint)face->glyph->advance.x
		};

		Characters.insert(std::pair<GLchar, FontChar>(character, fontChar));
	}

	// Destroy FreeType objects once we are finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

CFont::~CFont()
{
}

glm::ivec2 CFont::TextBounds(std::string text, float scale)
{
	glm::ivec2 boundry;
	int lines = 1;
	int currentX = 0;

	for (std::string::const_iterator character = text.begin(); character != text.end(); character++)
	{
		FontChar fontChar = Characters[*character];
		currentX += (int)std::roundf((fontChar.Advance >> 6) * scale);
		boundry.y = std::max(boundry.y, (int)std::roundf(fontChar.Size.y * scale + (fontChar.Bearing.y - fontChar.Size.y)));

		if (*character == '\n')
		{
			boundry.x = std::max(currentX, boundry.x);
			currentX = 0;
			lines++;
		}
	}

	boundry.x = std::max(currentX, boundry.x);
	boundry.y *= lines;
	return boundry;
}

void CFont::CleanUp()
{
	std::for_each(Fonts.begin(), Fonts.end(), [](std::pair<const std::string, CFont*>& it) { delete it.second; });
	Fonts.clear();
}

void CFont::GenerateFonts()
{
	for (const auto& entry : std::filesystem::directory_iterator(GetFilepath()))
	{
		std::string filename = entry.path().filename().replace_extension().u8string();
		std::string extension = entry.path().extension().u8string();

		IfThenStatement(extension == ".ttf", Fonts.insert({ filename + "Small", new CFont(filename + extension, 32) }));
		IfThenStatement(extension == ".ttf", Fonts.insert({ filename + "Big", new CFont(filename + extension, 64) }));
	}
}

CFont* CFont::GetFont(std::string fontName)
{
	auto it = Fonts.find(fontName);
	if (it != Fonts.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "WARNING: Failed to find FONT '" << fontName << "'" << std::endl;
		return nullptr;
	}
}

GLuint CFont::GenerateTexture(FT_Face face)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Linear?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Linear?

	return texture;
}

std::string CFont::GetFilepath()
{
	return (std::filesystem::current_path().parent_path()).string() + "\\Resources\\Fonts\\";
}
