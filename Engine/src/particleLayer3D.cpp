#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_inverse.hpp>

#include <algorithm>

#include "particleLayer3D.h"
#include "particle3D.h"
#include "helper.h"
#include "mesh.h"
#include "program.h"
#include "uniformManager.h"
#include "texture.h"

#include "camera3D.h"

CParticleLayer3D::CParticleLayer3D(std::shared_ptr<CCamera3D> cam)
{
	LayerType = "CParticleLayer3D";

	Camera = cam;

	ParticleProgram = CProgram::GetProgram("ParticleRender");
	Uniforms = new CUniformManager(ParticleProgram);

	Uniforms->UpdateUniformMat4R("PVM", CCamera::GetProjViewMatrix());
	Uniforms->UpdateUniformTexture("ParticleTexture", CTexture::GetTexture("Fire")->GetID());
	Uniforms->UpdateUniformFloat("Size", 0.1f);
	Uniforms->UpdateUniformfVec3R("vQuad1", Quad1);
	Uniforms->UpdateUniformfVec3R("vQuad2", Quad2);

	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Position Buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, MeshPoints.size() * sizeof(glm::vec3), MeshPoints.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

CParticleLayer3D::~CParticleLayer3D()
{
	std::for_each(Particles.begin(), Particles.end(), [](CParticle3D* obj) { delete obj; });
	Particles.clear();
}

void CParticleLayer3D::Render()
{
	// Set to additive blending
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);

	glUseProgram(ParticleProgram->GetProgramID());
	Uniforms->AssignUniforms();
	
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, MeshPoints.size());
	glBindVertexArray(0);

	Uniforms->UnbindTextures();
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	// Reset back to normal blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);

	//// Pre-load the view proj matrix
	//glm::mat4 viewProjMat = CCamera::GetProjViewMatrix();

	//// Bind program
	//glUseProgram(ColourProgram->GetProgramID());

	//// Bind mesh
	//glBindVertexArray(SquareMesh->GetVAO());
	//int tempIndicieCount = SquareMesh->GetIndicieCount();

	//GLuint pvmID = ColourProgram->GetUniform("PVM");
	//GLuint colourID = ColourProgram->GetUniform("objColour");

	//for (int i = 0; i < (int)Particles.size(); ++i)
	//{
	//	CParticle3D* particle = Particles[i];

	//	// Set the matrix
	//	glm::mat4 pvm = viewProjMat * particle->GetModelMatrix();


	//	glUniformMatrix4fv(pvmID, 1, GL_FALSE, glm::value_ptr(pvm));

	//	// Set the colour
	//	glm::vec3 aColour = particle->GetColour();
	//	glUniform4f(colourID, aColour.x, aColour.y, aColour.z, 1.0f);

	//	// Draw the particle
	//	glDrawElements(GL_TRIANGLES, tempIndicieCount, GL_UNSIGNED_INT, 0);
	//}

	//// Unbind mesh
	//glBindVertexArray(0);

	//// Unbind program
	//glUseProgram(0);
}

void CParticleLayer3D::Update()
{
	MeshPoints.clear();
	std::vector<CParticle3D*>::iterator it = Particles.begin();

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
			MeshPoints.push_back((*it)->GetLocation());
			++it;
		}
	}

	
	// Bind VAO
	glBindVertexArray(VAO);

	// Update VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, MeshPoints.size() * sizeof(glm::vec3), MeshPoints.data(), GL_DYNAMIC_DRAW);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	// Update billboarding effect
	glm::vec3 camFront = Camera->GetCamLookPos() - Camera->GetCamPos();
	camFront = glm::normalize(camFront);

	Quad1 = glm::cross(camFront, Camera->GetCamUpDir());
	Quad1 = glm::normalize(Quad1);

	Quad2 = glm::cross(camFront, Quad1);
	Quad2 = glm::normalize(Quad2);
}

void CParticleLayer3D::SpawnBurst(ParticleType3D& aType, glm::vec3 aLocation, int count)
{
	for (int i = 0; i < count; ++i)
	{
		SpawnOne(aType, aLocation);
	}
}

void CParticleLayer3D::SpawnOne(ParticleType3D& aType, glm::vec3 aLocation)
{
	glm::vec3 aColour = glm::vec3(aType.ColourCenter.r + (HELPER::Random() * 2.0f - 1.0f) * aType.ColourVariation,
								  aType.ColourCenter.g + (HELPER::Random() * 2.0f - 1.0f) * aType.ColourVariation,
								  aType.ColourCenter.b + (HELPER::Random() * 2.0f - 1.0f) * aType.ColourVariation);
	aColour = glm::clamp(aColour, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(255.0f, 255.0f, 255.0f));

	glm::vec3 aLoc = aLocation + HELPER::RandomVecInRadius3D(aType.LocationVariation);
	glm::vec3 aVelocity = aType.VelocityCenter + HELPER::RandomVecInRadius3D(aType.VelocityVariation);
	glm::vec3 anAcceleration = aType.AccelerationCenter + HELPER::RandomVecInRadius3D(aType.AccelerationVariation);

	float aSize = std::fmaxf(abs(aType.SizeCenter + (HELPER::Random() * 2.0f - 1.0f) * aType.SizeVariation), 1.0f);
	float aLifeSpan = abs(aType.LifeSpanCenter + (HELPER::Random() * 2.0f - 1.0f) * aType.LifeSpanVariation);

	Particles.push_back(new CParticle3D(Camera, aColour, aLoc, aVelocity, anAcceleration, aSize, aLifeSpan));
}
