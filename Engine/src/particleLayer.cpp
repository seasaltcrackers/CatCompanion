#include <gtc/type_ptr.hpp>

#include "particleLayer.h"
#include "particle.h"
#include "helper.h"
#include "mesh.h"
#include "program.h"
#include <algorithm>

#include "camera.h"


CParticleLayer::CParticleLayer()
{
	ColourProgram = CProgram::GetProgram("Colour");
	SquareMesh = CMesh::GetMesh("3D Square");
	LayerType = "CParticleLayer";
}

CParticleLayer::~CParticleLayer()
{
	std::for_each(Particles.begin(), Particles.end(), [](CParticle* obj) { delete obj; });
	Particles.clear();
}

void CParticleLayer::Render()
{
	// Pre-load the view proj matrix
	glm::mat4 viewProjMat = CCamera::GetProjViewMatrix();

	// Bind program
	glUseProgram(ColourProgram->GetProgramID());

	// Bind mesh
	glBindVertexArray(SquareMesh->GetVAO());
	int tempIndicieCount = SquareMesh->GetIndicieCount();

	GLuint pvmID = ColourProgram->GetUniform("PVM");
	GLuint colourID = ColourProgram->GetUniform("objColour");

	for (int i = 0; i < (int)Particles.size(); ++i)
	{
		CParticle* particle = Particles[i];

		// Set the matrix
		glm::mat4 pvm = viewProjMat * particle->GetModelMatrix();
		glUniformMatrix4fv(pvmID, 1, GL_FALSE, glm::value_ptr(pvm));

		// Set the colour
		glm::vec3 aColour = particle->GetColour();
		glUniform4f(colourID, aColour.x, aColour.y, aColour.z, 1.0f);

		// Draw the particle
		glDrawElements(GL_TRIANGLES, tempIndicieCount, GL_UNSIGNED_INT, 0);
	}

	// Unbind mesh
	glBindVertexArray(0);

	// Unbind program
	glUseProgram(0);
}

void CParticleLayer::Update()
{
	std::vector<CParticle*>::iterator it = Particles.begin();

	while (it != Particles.end())
	{
		(*it)->Update();

		if ((*it)->IsDead())
		{
			delete (*it);
			it = Particles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CParticleLayer::SpawnBurst(ParticleType& aType, glm::vec2 aLocation, int count)
{
	for (int i = 0; i < count; ++i)
	{
		SpawnOne(aType, aLocation);
	}
}

void CParticleLayer::SpawnOne(ParticleType& aType, glm::vec2 aLocation)
{
	glm::vec3 aColour = glm::vec3(aType.ColourCenter.r + (HELPER::Random() * 2.0f - 1.0f) * aType.ColourVariation,
								  aType.ColourCenter.g + (HELPER::Random() * 2.0f - 1.0f) * aType.ColourVariation,
								  aType.ColourCenter.b + (HELPER::Random() * 2.0f - 1.0f) * aType.ColourVariation);
	aColour = glm::clamp(aColour, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(255.0f, 255.0f, 255.0f));

	glm::vec2 aLoc = aLocation + HELPER::RandomVecInRadius(aType.LocationVariation);
	glm::vec2 aVelocity = aType.VelocityCenter + HELPER::RandomVecInRadius(aType.VelocityVariation);
	glm::vec2 anAcceleration = aType.AccelerationCenter + HELPER::RandomVecInRadius(aType.AccelerationVariation);

	float aSize = std::fmaxf(abs(aType.SizeCenter + (HELPER::Random() * 2.0f - 1.0f) * aType.SizeVariation), 1.0f);
	float aLifeSpan = abs(aType.LifeSpanCenter + (HELPER::Random() * 2.0f - 1.0f) * aType.LifeSpanVariation);

	Particles.push_back(new CParticle(aColour, aLoc, aVelocity, anAcceleration, aSize, aLifeSpan));
}
