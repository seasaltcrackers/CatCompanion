#include "demoSprite.h"
#include "helper.h"
#include "program.h"

CDemoSprite::CDemoSprite(CSprite* parentSprite, glm::vec2 location)
{
	Program = parentSprite->GetProgram();
	Location = glm::vec3(location, 0.0f);
	Scale = parentSprite->Scale;

	Animations = parentSprite->GetAnimations();
	CurrentAnimation = Animations.begin();
	PlayAnimation(CurrentAnimation->first);
}

void CDemoSprite::Update()
{
	std::string index = GetAnimationIndex();

	if (index == "")
	{
		IncrementAnimation();
	}

	CSprite::Update();
}


void CDemoSprite::IncrementAnimation()
{
	// Increment and loop
	CurrentAnimation++;
	IfThenStatement(CurrentAnimation == Animations.end(), CurrentAnimation = Animations.begin());
	PlayAnimation(CurrentAnimation->first);
}

void CDemoSprite::PlayAnimation(std::string name)
{
	SetAnimationIndex(name);
	SSpriteAnimation animation = GetAnimation(name); 

	Pivot = glm::vec3(glm::vec2(GetImageWidth(), GetImageHeight()) * glm::vec2(0.5f, 0.5f), 0.0f);
	UpdateModelMatrix();

	glm::value_ptr(glm::vec3());

	if (animation.AnimationType == EAnimationType::Loop ||
		animation.AnimationType == EAnimationType::PingPong)
	{
		SetAlarm(2.0f, std::bind(&CDemoSprite::IncrementAnimation, this));
	}
}

void CDemoSprite::OnAnimationEnd()
{
	IncrementAnimation();
}
