#pragma once
#include <glew.h>

#include "layer.h"

class CUniformManager;
class CProgram;


class GPU2DParticleLayer :
	public ILayer
{
public:
	GPU2DParticleLayer();
	~GPU2DParticleLayer();

	void Render() override;
	void Update() override;

private:
	CProgram* ComputeProgram;
	CProgram* ParticleProgram;

	CUniformManager* ComputeShaderVariables;
	CUniformManager* ParticleShaderVariables;

	GLuint PositionVBO;
};

