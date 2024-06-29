#include <iostream>

#include "sprite.h"
#include "helper.h"
#include "mesh.h"
#include "program.h"
#include "timer.h"

SSpriteAnimation::SSpriteAnimation()
{
}

SSpriteAnimation::SSpriteAnimation(CTexture* spriteSheet, int splitX, int splitY, glm::vec2 pivot, std::vector<glm::ivec2> frames, EAnimationType animationType, float fps, bool flipped)
{
	SpriteSheet = spriteSheet;
	SplitX = splitX;
	SplitY = splitY;
	Pivot = pivot;
	Frames = frames;
	AnimationType = animationType;
	FPS = fps;
	Flipped = flipped;
}

SSpriteAnimation::SSpriteAnimation(CTexture* spriteSheet, int splitX, int splitY, glm::vec2 pivot, EAnimationType animationType, float fps, bool flipped)
{
	SpriteSheet = spriteSheet;
	SplitX = splitX;
	SplitY = splitY;
	Pivot = pivot;
	AnimationType = animationType;
	FPS = fps;
	Flipped = flipped;

	for (int y = 0; y < splitY; ++y)
	{
		for (int x = 0; x < splitX; ++x)
		{
			Frames.push_back({ x, y });
		}
	}
}

/*
	Initialises a blank new sprite
*/
CSprite::CSprite() : CObject(CProgram::GetProgram("SpriteSheet"), CMesh::GetMesh("2D Square"))
{
	AnimationSpeed = 1.0f;
	AnimationFrame = 0.0f;
	GenerateAnimations();
}

/*
	Initialises a new sprite with the given animation as a default animation

	@param animation A list of textures
	@param aLoc The location the sprite will be spawned at
*/
CSprite::CSprite(SSpriteAnimation animation, glm::vec3 aLoc) : CObject(CProgram::GetProgram("SpriteSheet"), CMesh::GetMesh("2D Square"), aLoc)
{
	// Create a default animation
	Animations.insert({ "Default", animation });
	SetAnimationIndex("Default");
}

/*
	Destroys the sprites dynamic memory
*/
CSprite::~CSprite()
{
	Animations.clear();
}

/*
	Updates the sprites animation and parent function
*/
void CSprite::Update()
{
	CObject::Update(); // Update the model matrix

	IfThenReturn(AnimationIndex == "", ); // If the animation index is empty then break out (don't update the animation)
	SSpriteAnimation tempAnim = Animations[AnimationIndex];
	AnimationFrame += tempAnim.FPS * AnimationSpeed * TIMER::GetDeltaSeconds(); // Increase animation frame

	// Check if animations is finished
	if (tempAnim.AnimationType == EAnimationType::Toggle && AnimationFrame >= tempAnim.Frames.size())
	{
		// Stop the animation
		AnimationFrame = (float)tempAnim.Frames.size() - 1.0f;
		AnimationSpeed = 0.0f;
		OnAnimationEnd();
	}
}

/*
	Draw the sprite based on the current frame and animation
*/
void CSprite::Draw(glm::mat4& pv)
{
	IfThenReturn(AnimationIndex == "", ); // If the animation index is empty then break out (don't update the animation)

	// Update uniforms
	SSpriteAnimation tempAnim = Animations[AnimationIndex];
	UpdateUniformTexture("tex", tempAnim.SpriteSheet->GetID());
	UpdateUniformiVec2("splitNumber", { tempAnim.SplitX, tempAnim.SplitY });
	UpdateUniformiVec2("spriteIndex", tempAnim.Frames[CalculateFrame()]);

	// Draw the sprite
	CObject::Draw(pv);
}

/*
	Returns the width of a single image from the current animation

	@return The width of a sub-image
*/
float CSprite::GetImageWidth()
{
	IfThenReturn(AnimationIndex == "", 0); // If the animation index is empty then break out (don't update the animation)
	SSpriteAnimation tempAnim = Animations[AnimationIndex];
	return tempAnim.SpriteSheet->GetWidth() / static_cast<float>(tempAnim.SplitX);
}

/*
	Returns the height of a single image from the current animation

	@return The height of a sub-image
*/
float CSprite::GetImageHeight()
{
	IfThenReturn(AnimationIndex == "", 0); // If the animation index is empty then break out (don't update the animation)
	SSpriteAnimation tempAnim = Animations[AnimationIndex];
	return tempAnim.SpriteSheet->GetHeight() / static_cast<float>(tempAnim.SplitY);
}

/*
	Sets the new animation based on the name

	@param anAnimationIndex The name index of the new animation
*/
void CSprite::SetAnimationIndex(std::string anAnimationIndex)
{
	// Test if it is a new animation index
	if (AnimationIndex != anAnimationIndex)
	{
		// Reset animations
		AnimationIndex = anAnimationIndex;
		AnimationFrame = 0;
		AnimationSpeed = 1;

		IfThenReturn(AnimationIndex == "", ); // If the animation index is empty then break out (don't update the animation)
		Pivot = glm::vec3(Animations[AnimationIndex].Pivot, 0.0f); // Update the pivot based on the animation

		UpdateModelMatrix();
	}
}

/*
	Gets the current animation index

	@return The name index of the current animation
*/
std::string CSprite::GetAnimationIndex()
{
	return AnimationIndex;
}

SSpriteAnimation& CSprite::GetAnimation(std::string anAnimationIndex)
{
	return Animations[anAnimationIndex];
}

std::map<std::string, SSpriteAnimation>& CSprite::GetAnimations()
{
	return Animations;
}

/*
	Calculates the current frame based on animation type, FPS ect

	@return The current frame (returns -1 if no animation is selected)
*/
int CSprite::CalculateFrame()
{
	IfThenReturn(AnimationIndex == "", -1); // If the animation index is empty then break out (don't update the animation)
	SSpriteAnimation tempAnim = Animations[AnimationIndex];
	int frameSize = tempAnim.Frames.size();

	// Return the frame based on the animation type
	switch (tempAnim.AnimationType)
	{
		case (EAnimationType::Toggle):
		case (EAnimationType::Loop):
		{
			return static_cast<int>(floorf(fmodf(AnimationFrame, (float)frameSize)));
			break;
		}
		case (EAnimationType::PingPong):
		{
			float value = fmodf(AnimationFrame, (frameSize - 1) * 2.0f);
			return static_cast<int>(std::roundf(value - std::fmaxf(value - (frameSize - 1), 0.0f) * 2));
			break;
		}
	}

	return -1;
}

/*
	Finds the number of frames in the current animation

	@return The number of frames in the current animation (returns 0 if no animation selected)
*/
int CSprite::GetFrameCount()
{
	IfThenReturn(AnimationIndex == "", 0); // If the animation index is empty then break out (don't update the animation)
	return Animations[AnimationIndex].Frames.size();
}

/*
	A virtual function for child objects to generate their animations
*/
void CSprite::GenerateAnimations() { }

/*
	A virtual function for child objects to utilise that is called when a toggle animation ends
*/
void CSprite::OnAnimationEnd() { }

/*
	Calculates and updates the current model matrix (automaticaly scales to image width and height)
*/
void CSprite::UpdateModelMatrix()
{
	glm::mat4 spriteScale = glm::scale(glm::mat4(), glm::vec3(GetImageWidth(), GetImageHeight(), 1.0f));
	glm::mat4 pivot = glm::translate(glm::mat4(), -Pivot);
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(Scale.x * (Animations[AnimationIndex].Flipped ? -1.0f : 1.0f), Scale.y, Scale.z));
	glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(RotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translation = glm::translate(glm::mat4(), Location);

	ModelMatrix = translation * rotation * scale * pivot * spriteScale;
}
