#pragma once
#include <glm.hpp>
#include <vector>
#include <iostream>
#include <glew.h>

#include "layer.h"

class CParticle3D;
class CMesh;
class CProgram;
class CCamera3D;
class CUniformManager;

struct ParticleType3D
{
	glm::vec3 ColourCenter = glm::vec3();
	float ColourVariation = 0.0f;

	float LocationVariation = 0.0f;

	glm::vec3 VelocityCenter = glm::vec3();
	float VelocityVariation = 0.0f;

	glm::vec3 AccelerationCenter = glm::vec3();
	float AccelerationVariation = 0.0f;

	float SizeCenter = 10.0f;
	float SizeVariation = 0.0f;

	float LifeSpanCenter = 1.0f;
	float LifeSpanVariation = 0.0f;
};


class CParticleLayer3D :
	public ILayer
{
public:
	CParticleLayer3D(std::shared_ptr<CCamera3D> cam);
	~CParticleLayer3D();

	void Render() override;
	void Update() override;

	void SpawnBurst(ParticleType3D& aType, glm::vec3 aLocation, int count);
	void SpawnOne(ParticleType3D& aType, glm::vec3 aLocation);

private:
	std::shared_ptr<CCamera3D> Camera;
	std::vector<CParticle3D*> Particles;

	GLuint VAO;
	GLuint VBO;
	std::vector<glm::vec3> MeshPoints;

	CProgram* ParticleProgram;
	CUniformManager* Uniforms;
	
	glm::vec3 Quad1;
	glm::vec3 Quad2;
	//CMesh* SquareMesh;
};