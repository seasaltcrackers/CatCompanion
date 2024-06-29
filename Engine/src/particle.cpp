#include "particle.h"
#include "timer.h"
#include "graphics.h"

CParticle::CParticle(glm::vec3 colour, glm::vec2 location, glm::vec2 velocity, glm::vec2 acceleration, float size, float lifeSpan)
{
	Colour = colour;

	Location = location;
	Velocity = velocity;
	Acceleration = acceleration;

	Size = size;
	OriginalSize = size;

	LifeSpan = lifeSpan;
	OriginalLifeSpan = lifeSpan;

	ModelMatrix = GFX::CalculateModelMatrix(Location, glm::vec2(Size, Size));
}

void CParticle::Update()
{
	float dSecs = TIMER::GetDeltaSeconds();
	LifeSpan -= dSecs;

	if (LifeSpan > 0.0f)
	{
		Velocity += Acceleration * dSecs;
		Location += Velocity * dSecs;
		Size = OriginalSize * (LifeSpan / OriginalLifeSpan);

		ModelMatrix = GFX::CalculateModelMatrix(Location, glm::vec2(Size, Size));
	}
	else
	{
		Size = 0.0f;
	}
}

glm::vec3 CParticle::GetColour()
{
	return Colour / glm::vec3(255.0f, 255.0f, 255.0f);
}

glm::mat4 CParticle::GetModelMatrix()
{
	return ModelMatrix;
}

bool CParticle::IsDead()
{
	return Size == 0.0f;
}
