#pragma once
#include <glm.hpp>
#include <iostream>

class CCamera3D;

class CParticle3D
{
public:
	CParticle3D(std::shared_ptr<CCamera3D> cam, glm::vec3 colour, glm::vec3 location, glm::vec3 velocity, glm::vec3 acceleration, float size, float lifeSpan);

	void Update();

	glm::vec3 GetLocation();
	glm::vec3 GetColour();
	glm::mat4 GetModelMatrix();

	bool IsDead();

private:
	std::shared_ptr<CCamera3D> Camera;

	glm::vec3 Colour;

	glm::vec3 Location;
	glm::vec3 Velocity;
	glm::vec3 Acceleration;

	float Size;
	float OriginalSize;

	float LifeSpan;				// Time in seconds
	float OriginalLifeSpan;		// Time in seconds

	glm::mat4 ModelMatrix;
};

