#include "texturedParticleLayer.h"

#include <gtc/type_ptr.hpp>

#include "helper.h"
#include "timer.h"
#include "program.h"
#include "mesh.h"
#include "camera.h"
#include "graphics.h"

CTexturedParticleLayer::CTexturedParticleLayer()
{
	TextureProgram = CProgram::GetProgram("Texture");
	SquareMesh = CMesh::GetMesh("3D Square");
	LayerType = "CTexturedParticleLayer";
}

void CTexturedParticleLayer::Update()
{
	float dt = TIMER::GetDeltaSeconds();

	std::list<TexturedParticle>::iterator it = Particles.begin();
	while (it != Particles.end())
	{
		it->LifeTime -= dt;

		if (it->LifeTime <= 0.0f)
		{
			it = Particles.erase(it);
		}
		else
		{
			it->Velocity += it->Acceleration * dt;
			it->Location += it->Velocity * dt;
			it->Scale = (it->LifeTime / it->OriginalLifeTime) * it->OriginalScale;
			++it;
		}
	}
}

void CTexturedParticleLayer::Render()
{
	// Pre-load the view proj matrix
	glm::mat4 viewProjMat = CCamera::GetProjViewMatrix();

	// Bind program
	glUseProgram(TextureProgram->GetProgramID());

	glActiveTexture(GL_TEXTURE0);

	// Bind mesh
	glBindVertexArray(SquareMesh->GetVAO());
	int tempIndicieCount = SquareMesh->GetIndicieCount();

	GLuint pvmID = TextureProgram->GetUniform("PVM");
	GLuint texID = TextureProgram->GetUniform("tex");

	for (TexturedParticle& p : Particles)
	{
		// Set the matrix
		glm::mat4 pvm = viewProjMat * GFX::CalculateModelMatrix(p.Location, glm::vec2(p.Scale, -p.Scale));
		glUniformMatrix4fv(pvmID, 1, GL_FALSE, glm::value_ptr(pvm));

		// Set the colou
		glBindTexture(GL_TEXTURE_2D, p.TextureID);
		glUniform1i(texID, 0); 

		// Draw the particle
		glDrawElements(GL_TRIANGLES, tempIndicieCount, GL_UNSIGNED_INT, 0);
	}

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void CTexturedParticleLayer::AddParticleType(std::string name, TexturedParticleType type)
{
	ParticleTypes.insert({ name, type });
}

void CTexturedParticleLayer::SpawnBurst(glm::vec2 location, std::string particleType, int count)
{
	TexturedParticleType type = ParticleTypes[particleType];
	for (int i = 0; i < count; ++i) { SpawnSpecificParticle(location, type); }
}

void CTexturedParticleLayer::SpawnParticle(glm::vec2 location, std::string particleType)
{
	SpawnSpecificParticle(location, ParticleTypes[particleType]);
}

void CTexturedParticleLayer::SpawnSpecificParticle(glm::vec2 location, TexturedParticleType type)
{
	TexturedParticle p;

	p.Location = location + HELPER::RandomVecInRadius(type.LocationVariation);
	p.Velocity = type.VelocityCenter + HELPER::RandomVecInRadius(type.VelocityVariation);
	p.Acceleration = type.AccelerationCenter + HELPER::RandomVecInRadius(type.AccelerationVariation);
	
	p.Scale = type.ScaleCenter + HELPER::Random(-type.ScaleVariation, type.ScaleVariation);
	p.OriginalScale = p.Scale;

	p.LifeTime = type.LifeTimeCenter + HELPER::Random(-type.LifeTimeVariation, type.LifeTimeVariation);
	p.OriginalLifeTime = p.LifeTime;
	
	p.TextureID = type.TextureID;

	Particles.emplace_back(p);
}
