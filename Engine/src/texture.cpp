#include <SOIL.h>
#include <algorithm>
#include <filesystem>
#include <iostream>

#include "helper.h"
#include "texture.h"

// The begining file path for all textures
const char ImageFilePath[] = "Resources\\Images\\";

std::map<std::string, CTexture*> CTexture::TextureIDs; // All Texture IDs


/*
	Generates a new texture based on the filename

	@param filename The file location of your texture, assume "Resources\\Images\\" has been added on already
*/
CTexture::CTexture(std::string filename)
{
	std::cout << "Asset " << filename << " has been loaded" << std::endl;

	Filepath = filename;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Texture will mirror repeat on the x and y axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Nearest pixel will be chosen (good for pixel art)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* image = SOIL_load_image((ImageFilePath + filename).c_str(), &Width, &Height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);
}

CTexture::CTexture(void* data, unsigned int width, unsigned int height, GLenum format, GLenum internalFormat)
{
	Filepath = "";
	Width = width;
	Height = height;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
	
	// Texture will mirror repeat on the x and y axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Nearest pixel will be chosen (good for pixel art)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

CTexture::CTexture(std::string filename, unsigned int width, unsigned int height, GLuint id)
{
	Filepath = filename;
	Width = width;
	Height = height;
	ID = id;
}

CTexture::CTexture(unsigned int width, unsigned int height, bool rgb)
{
	Filepath = "";
	Width = width;
	Height = height;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Create an empty data array to create a blank texture
	int size = Width * Height * (rgb ? 3 : 4);// *sizeof(GLuint)
	GLuint* empty = new GLuint[size];
	std::fill(empty, empty + size, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, rgb ? GL_RGB : GL_RGBA, Width, Height, 0, rgb ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, empty);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

std::string CTexture::GetFilepath()
{
	return Filepath;
}

/*
	Gets the Texture ID of the texture

	@return The GLuint ID of the texture 
*/
GLuint CTexture::GetID()
{
	return ID;
}

/*
	Gets the width of the texture

	@return The int width of the texture
*/
int CTexture::GetWidth()
{
	return Width;
}

/*
	Gets the height of the texture

	@return The int height of the texture
*/
int CTexture::GetHeight()
{
	return Height;
}

glm::ivec2 CTexture::GetDimensions()
{
	return glm::vec2(Width, Height);
}

/*
	Cleans up the dynamic memory caused by the TextureIDs
*/
void CTexture::CleanUp()
{
	std::for_each(TextureIDs.begin(), TextureIDs.end(), [](std::pair<const std::string, CTexture*>& it) { delete it.second; });
	TextureIDs.clear();
}

void CTexture::GenerateTextures()
{
	for (const auto& entry : std::filesystem::directory_iterator(ImageFilePath))
	{
		std::string filename = entry.path().filename().replace_extension().u8string();
		std::string extension = entry.path().extension().u8string();
		IfThenStatement(extension == ".png" || extension == ".jpg", TextureIDs.insert({ filename, new CTexture(filename + extension) }));
	}
}

CTexture* CTexture::GetTexture(std::string textureName)
{
	auto it = TextureIDs.find(textureName);
	if (it != TextureIDs.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "WARNING: Failed to find TEXTURE '" << textureName << "'" << std::endl;
		return nullptr;
	}
}

void CTexture::SetTexture(std::string textureName, CTexture* texture)
{
	auto it = TextureIDs.find(textureName);
	if (it != TextureIDs.end())
	{
		GLuint id = it->second->GetID();
		glDeleteTextures(1, &id);

		delete it->second;
		it->second = texture;
	}
	else
	{
		TextureIDs.insert({ textureName, texture });
	}
}
