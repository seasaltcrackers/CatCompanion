#pragma once
#include <glm.hpp>

class CParticle
{
public:
	CParticle(glm::vec3 colour, glm::vec2 location, glm::vec2 velocity, glm::vec2 acceleration, float size, float lifeSpan);

	void Update();

	glm::vec3 GetColour();
	glm::mat4 GetModelMatrix();

	bool IsDead();

private:
	glm::vec3 Colour;
	
	glm::vec2 Location;
	glm::vec2 Velocity;
	glm::vec2 Acceleration;

	float Size;
	float OriginalSize;

	float LifeSpan;				// Time in seconds
	float OriginalLifeSpan;		// Time in seconds

	glm::mat4 ModelMatrix;
};

