#pragma once
#include <glew.h>
#include <glm.hpp>

enum class FBOType
{
	DEPTH_BUFFER,
	COLOUR_BUFFER,
};

class CMesh;
class CProgram;

class CFBOObject
{
public:
	CFBOObject(FBOType type);
	~CFBOObject();

public:
	void StartCapture();
	void EndCapture();
	GLuint GetTexture();
	void Render();

private:
	glm::mat4 IdentityMat;

	CMesh* FBOMesh;
	CProgram* FBOProg;

	FBOType FrameBufferType;

	GLuint Texture;
	GLuint FBO;

	GLsizei TextureWidth;
	GLsizei TextureHeight;
};