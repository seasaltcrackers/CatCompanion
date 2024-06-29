#include <iostream>
#include <gtc\type_ptr.hpp>

#include "fboObject.h"
#include "camera.h"
#include "mesh.h"
#include "program.h"


CFBOObject::CFBOObject(FBOType type)
{
	FrameBufferType = type;
	FBOMesh = CMesh::GetMesh("FBO Mesh");
	FBOProg = CProgram::GetProgram("Texture");
	IdentityMat = glm::mat4(1.0f);

	// Varying variables
	GLenum textureFormat = 0;
	GLenum textureType = 0;
	GLenum attachmentType = 0;

	// Assign based on type
	switch (type)
	{
	case FBOType::COLOUR_BUFFER:
	{
		textureFormat = GL_RGB;
		textureType = GL_UNSIGNED_BYTE;
		attachmentType = GL_COLOR_ATTACHMENT0;
		TextureWidth = CCamera::GetScreenWidth();
		TextureHeight = CCamera::GetScreenHeight();
		break;
	}
	case FBOType::DEPTH_BUFFER:
	{
		textureFormat = GL_DEPTH_COMPONENT;
		textureType = GL_FLOAT;
		attachmentType = GL_DEPTH_ATTACHMENT;
		TextureWidth = 8096;
		TextureHeight = 8096;
		break;
	}
	default:
	{
		std::cout << "FBO type is unsupported" << std::endl;
	}
	}


	// Setup the texture
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, TextureWidth, TextureHeight, 0, textureFormat, textureType, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Setup the frame buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, Texture, 0);

	switch (type)
	{
	case FBOType::DEPTH_BUFFER:
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		break;
	}
	case FBOType::COLOUR_BUFFER:
	{
		// Generate a render buffer
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, TextureWidth, TextureHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		break;
	}
	}

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete: " << fboStatus << std::endl;

	// Unbind
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CFBOObject::~CFBOObject()
{
}

void CFBOObject::StartCapture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	switch (FrameBufferType)
	{
	case FBOType::DEPTH_BUFFER:
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		break;
	}
	case FBOType::COLOUR_BUFFER:
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		break;
	}
	}

	glViewport(0, 0, TextureWidth, TextureHeight);
}


void CFBOObject::EndCapture()
{
	glFlush();
	glFinish();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CCamera::GetScreenWidth(), CCamera::GetScreenHeight());
}

GLuint CFBOObject::GetTexture()
{
	return Texture;
}

void CFBOObject::Render()
{
	// Use the program
	glUseProgram(FBOProg->GetProgramID());

	// Assign the PVM matrix uniform
	glUniformMatrix4fv(FBOProg->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(IdentityMat));

	// Assign texture uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(FBOProg->GetUniform("tex"), 0);

	// Draw the mesh
	FBOMesh->Draw();

	// Unbind
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
