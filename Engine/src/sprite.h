#pragma once
#include <vector>
#include <map>

#include "object.h"
#include "texture.h"

enum class EAnimationType
{
	Toggle,
	Loop,
	PingPong,
};

struct SSpriteAnimation
{
	CTexture* SpriteSheet = nullptr; // The ID of the sprite sheet for the animation
	int SplitX = 0; // The number of sprites going horizontaly on the sprite sheet
	int SplitY = 0; // The number of sprites going verticaly on the sprite sheet
	glm::vec2 Pivot = glm::vec2(); // The pivot of one frame
	std::vector<glm::ivec2> Frames; // A list of frames which map to sprite sheet coords
	EAnimationType AnimationType; // The type of animation (What happens after it is done etc.)
	float FPS = 0.0f; // The frames per second of the animation
	bool Flipped = false; // Whether the sprite is flipped on the x-axis

	SSpriteAnimation();
	SSpriteAnimation(CTexture* spriteSheet, int splitX, int splitY, glm::vec2 pivot, std::vector<glm::ivec2> frames, EAnimationType animationType, float fps, bool flipped = false);
	SSpriteAnimation(CTexture* spriteSheet, int splitX, int splitY, glm::vec2 pivot, EAnimationType animationType, float fps, bool flipped = false);
};

class CSprite :
	public CObject
{
public:
	CSprite();
	CSprite(SSpriteAnimation animation, glm::vec3 aLoc); // Initialise new sprite with default animation
	virtual ~CSprite();

	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;

	float GetImageWidth(); // Find sub sprite width
	float GetImageHeight(); // Find sub sprite width
	int GetFrameCount(); // Find the number of frames in the current animation

	void SetAnimationIndex(std::string anAnimationIndex); // Set the current animation
	std::string GetAnimationIndex(); // Get the current animation index
	SSpriteAnimation& GetAnimation(std::string anAnimationIndex);
	std::map<std::string, SSpriteAnimation>& GetAnimations();

	int CalculateFrame(); // Get the current index of the animation

	virtual void GenerateAnimations();
	virtual void OnAnimationEnd();

protected:
	std::map<std::string, SSpriteAnimation> Animations; // List of Animations
	float AnimationSpeed; // The speed of the animation (1.0f is normal speed)
	float AnimationFrame; // The current frame of the animation (number is rounded to get the frame

	void UpdateModelMatrix() override;

private:
	std::string AnimationIndex; // The index of the current animation defaults to ""
};
