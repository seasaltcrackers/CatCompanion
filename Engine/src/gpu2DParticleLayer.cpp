#include <vector>
#include <glm/glm.hpp>

#include "gpu2DParticleLayer.h"

#include "uniformManager.h"
#include "program.h"
#include "helper.h"
#include "texture.h"
#include "camera.h"

#define NUM_PARTICLES 128

GPU2DParticleLayer::GPU2DParticleLayer()
{
	ComputeProgram = CProgram::GetProgram("2DParticleCompute");
	ParticleProgram = CProgram::GetProgram("2DParticle");

	ComputeShaderVariables = new CUniformManager(ComputeProgram);
	ParticleShaderVariables = new CUniformManager(ParticleProgram);

	//ParticleShaderVariables->UpdateUniformTexture("ParticleTexture", );
	ParticleShaderVariables->UpdateUniformFloat("Size", 10.0f);
	ParticleShaderVariables->UpdateUniformMat4R("PVM", CCamera::GetProjViewMatrix());

	// Generate starting values
	std::vector<glm::vec2> initialPosition(NUM_PARTICLES);

	for (int i = 0; i < NUM_PARTICLES; ++i)
	{
		initialPosition[i] = glm::vec2(HELPER::Random(0, 1920), 500);
	}

	// Position Buffer
	// ------------------------
	glGenBuffers(1, &PositionVBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PositionVBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, initialPosition.size() * sizeof(glm::vec2), &initialPosition[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, PositionVBO);
	// ------------------------

	// VAO
	// ------------------------
	GLuint vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, PositionVBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, NULL, 0);
	glEnableVertexAttribArray(0);

	// Clear Binds
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// ------------------------
}

GPU2DParticleLayer::~GPU2DParticleLayer()
{
	delete ComputeShaderVariables;
	ComputeShaderVariables = nullptr;

	delete ParticleShaderVariables;
	ParticleShaderVariables = nullptr;
}

void GPU2DParticleLayer::Render()
{
	// Make sure the particles have finished updating
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Set to additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Bind program and update uniforms
	glUseProgram(ParticleProgram->GetProgramID());
	ParticleShaderVariables->AssignUniforms();

	// Bind position buffer
	glBindBuffer(GL_ARRAY_BUFFER, PositionVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, NULL, 0);
	glEnableVertexAttribArray(0);

	// Render
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);


	// Reset back to normal blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Unbind things
	ParticleShaderVariables->UnbindTextures();
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

}

void GPU2DParticleLayer::Update()
{
	glUseProgram(ComputeProgram->GetProgramID());
	ComputeShaderVariables->AssignUniforms();
	glDispatchCompute(NUM_PARTICLES / 128, 1, 1);
}
