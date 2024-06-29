#pragma once
#include <glm.hpp>
#include <vector>

#include "layer.h"

class CParticle;
class CMesh;
class CProgram;

struct ParticleType
{
	glm::vec3 ColourCenter = glm::vec3();
	float ColourVariation = 0.0f;

	float LocationVariation = 0.0f;

	glm::vec2 VelocityCenter = glm::vec2();
	float VelocityVariation = 0.0f;

	glm::vec2 AccelerationCenter = glm::vec2();
	float AccelerationVariation = 0.0f;

	float SizeCenter = 10.0f;
	float SizeVariation = 0.0f;

	float LifeSpanCenter = 1.0f;
	float LifeSpanVariation = 0.0f;
};


class CParticleLayer :
	public ILayer
{
public:
	CParticleLayer();
	virtual ~CParticleLayer();

	void Render() override;
	void Update() override;

	void SpawnBurst(ParticleType& aType, glm::vec2 aLocation, int count);
	void SpawnOne(ParticleType& aType, glm::vec2 aLocation);

private:
	std::vector<CParticle*> Particles;

	CProgram* ColourProgram;
	CMesh* SquareMesh;
};