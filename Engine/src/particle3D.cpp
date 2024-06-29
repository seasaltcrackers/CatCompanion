#include <gtc/matrix_transform.hpp>

#include "particle3D.h"
#include "timer.h"
#include "graphics.h"
#include "camera3D.h"
#include "helper.h"


CParticle3D::CParticle3D(std::shared_ptr<CCamera3D> cam, glm::vec3 colour, glm::vec3 location, glm::vec3 velocity, glm::vec3 acceleration, float size, float lifeSpan)
{
	Camera = cam;

	Colour = colour;

	Location = location;
	Velocity = velocity;
	Acceleration = acceleration;

	Size = size;
	OriginalSize = size;

	LifeSpan = lifeSpan;
	OriginalLifeSpan = lifeSpan;

	ModelMatrix = GFX::CalculateModelMatrix(Location, glm::vec3(Size, Size, Size));
}

void CParticle3D::Update()
{
	float dSecs = TIMER::GetDeltaSeconds();
	LifeSpan -= dSecs;

	if (LifeSpan > 0.0f)
	{
		Velocity += Acceleration * dSecs;
		Location += Velocity * dSecs;
		Size = OriginalSize * (LifeSpan / OriginalLifeSpan);

		glm::vec3 forward = Location - Camera->GetCamPos();
		glm::vec2 tempVec = glm::vec2(forward.x, forward.z);
		float rotation = -HELPER::PointDirectionDeg({ 0, 0 }, tempVec) - 90.0f;

		ModelMatrix = GFX::CalculateModelMatrix(Location, { Size, Size, Size }, 0, rotation, 0);
	}
	else
	{
		Size = 0.0f;
	}
}

glm::vec3 CParticle3D::GetLocation()
{
	return Location;
}

glm::vec3 CParticle3D::GetColour()
{
	return Colour / glm::vec3(255.0f, 255.0f, 255.0f);
}

glm::mat4 CParticle3D::GetModelMatrix()
{
	return ModelMatrix;
}

bool CParticle3D::IsDead()
{
	return Size == 0.0f;
}
