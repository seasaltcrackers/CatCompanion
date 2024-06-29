#pragma once
#include <glew.h>
#include <string>
#include <map>
#include <glm.hpp>

class CTexture
{
public:
	CTexture(std::string filename);
	CTexture(void* data, unsigned int width, unsigned int height, GLenum format, GLenum internalFormat);
	CTexture(std::string filename, unsigned int width, unsigned int height, GLuint id);
	CTexture(unsigned int width, unsigned int height, bool rgb = false);

	std::string GetFilepath();
	GLuint GetID();

	int GetWidth();
	int GetHeight();
	glm::ivec2 GetDimensions();

	static void CleanUp();
	static void GenerateTextures();
	static CTexture* GetTexture(std::string textureName);
	static void SetTexture(std::string textureName, CTexture* texture);
private:
	std::string Filepath;

	GLuint ID;
	int Width;
	int Height;

	static std::map<std::string, CTexture*> TextureIDs; // All Texture IDs
};
