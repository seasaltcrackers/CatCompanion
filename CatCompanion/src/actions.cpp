#include <random>

#include "monitorManager.h"
#include "actions.h"

#include "windowsManager.h"
#include "timer.h"
#include "input.h"
#include "helper.h"
#include "cat.h"

SleepingAction::SleepingAction(CCat* parentCat, float seconds) :
	Action(parentCat)
{
	Seconds = seconds;
}

void SleepingAction::Start()
{
	StartTime = std::chrono::steady_clock::now();

	ParentCat->SetAnimationIndex("Sleeping");
	ParentCat->SetCurrentState(CatState::Sleeping);
}

bool SleepingAction::Update()
{
	float duration = (float)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - StartTime).count();
	return duration >= Seconds;
}

WalkTimedAction::WalkTimedAction(CCat* parentCat, DIRECTION direction, float seconds) :
	Action(parentCat)
{
	WalkDirection = direction;
	Seconds = seconds;
}

WalkTimedAction::~WalkTimedAction()
{
	ParentCat->Velocity.x = 0.0;
}

void WalkTimedAction::Start()
{
	StartTime = std::chrono::steady_clock::now();

	ParentCat->Velocity.x = (float)WalkDirection * ParentCat->WalkSpeed;
	ParentCat->SetAnimationIndex("Walking " + std::string(WalkDirection == DIRECTION::RIGHT ? "Right" : "Left"));
	ParentCat->SetCurrentState(CatState::Walking);
}

bool WalkTimedAction::Update()
{
	float duration = (float)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - StartTime).count();

	if (duration >= Seconds)
	{
		ParentCat->Velocity.x = 0.0f;
		return true;
	}
	else if (ParentCat->Velocity.x == 0.0f)
	{
		ParentCat->ClearSubsequentActions();
		return true;
	}
	else
	{
		return false;
	}
}

WalkTargetAction::WalkTargetAction(CCat* parentCat, float targetX) :
	Action(parentCat)
{
	TargetX = targetX;
	WalkDirection = DIRECTION::RIGHT;
}

WalkTargetAction::~WalkTargetAction()
{
	ParentCat->Velocity.x = 0.0f;
}

void WalkTargetAction::Start()
{
	WalkDirection = TargetX >= ParentCat->Location.x ? DIRECTION::RIGHT : DIRECTION::LEFT;

	ParentCat->Velocity.x = (float)WalkDirection * ParentCat->WalkSpeed;
	ParentCat->SetAnimationIndex("Walking " + std::string(WalkDirection == DIRECTION::RIGHT ? "Right" : "Left"));
	ParentCat->SetCurrentState(CatState::Walking);
}

bool WalkTargetAction::Update()
{
	float dist = std::fabsf(ParentCat->Location.x - TargetX);

	if (dist <= 2.0f)
	{
		ParentCat->Location.x = TargetX;
		ParentCat->Velocity.x = 0.0f;
		return true;
	}
	else if (ParentCat->Velocity.x == 0.0f)
	{
		ParentCat->ClearSubsequentActions();
		return true;
	}

	return false;
}

JumpingAction::JumpingAction(CCat* parentCat, float height) :
	Action(parentCat)
{
	JumpHeight = height + OverJumpOffset;
	TargetY = 0.0f;
	Window = NULL;
}

JumpingAction::JumpingAction(CCat* parentCat, HWND window) :
	Action(parentCat)
{
	JumpHeight = 0.0f;
	TargetY = 0.0f;
	Window = window;
}

void JumpingAction::QueueJumpWaitFunction(CCat* parentCat, HWND window, float targetY)
{
	CWindowsManager& WM = CWindowsManager::GetInstance();

	parentCat->QueueAction(
		new WaitAction(parentCat, [parentCat, window, targetY, &WM]()
	{
		float topValue = targetY;

		// Retrieve the bounds of the window
		RECT bounds;
		if (WM.GetWindowBoundry(window, bounds) && parentCat->Velocity.y < 0.0f)
		{
			topValue = bounds.top;
		}
		else if (window != NULL)
		{
			// Window no longer exists
			parentCat->ClearSubsequentActions();
			return true;
		}

		// Return once within 10 pixels
		return (parentCat->Location.y - topValue) < 10.0f;

	}));
}

void JumpingAction::QueueJumpingActions(CCat* parentCat, float targetY)
{
	// Calculate the time to reach the peak of the jump:
	// h = 0.5 * g * t ^ 2
	// sqrt((h * 2) / g) = t
	float jumpDuration = std::sqrtf((targetY * 2.0f) / parentCat->Acceleration.y);

	parentCat->QueueAction(new FunctionAction([parentCat, targetY]() { if (!parentCat->IsLocationValid(glm::vec3(parentCat->Location.x, targetY, 0.0f))) { parentCat->ClearSubsequentActions(); }; }));
	parentCat->QueueAction(new FunctionAction([parentCat]() { parentCat->SetCurrentState(CatState::Jumping); }));
	parentCat->QueueAction(new PlayAnimationAction(parentCat, "Jumping Start"));
	parentCat->QueueAction(new JumpingAction(parentCat, targetY));
	parentCat->QueueAction(new PlayAnimationAction(parentCat, "Jumping Middle"));
	JumpingAction::QueueJumpWaitFunction(parentCat, NULL, targetY);
	parentCat->QueueAction(new PlayAnimationAction(parentCat, "Jumping End"));
}

void JumpingAction::QueueJumpingActions(CCat* parentCat, HWND window)
{
	CWindowsManager& WM = CWindowsManager::GetInstance();

	parentCat->QueueAction(new FunctionAction([parentCat, window, &WM]() 
	{ 
		RECT bounds;
		WM.GetWindowBoundry(window, bounds);

		if (!parentCat->IsLocationValid(glm::vec3(parentCat->Location.x, bounds.top, 0.0f))) { parentCat->ClearSubsequentActions(); }; 
	}));

	parentCat->QueueAction(new FunctionAction([parentCat]() { parentCat->SetCurrentState(CatState::Jumping); }));
	parentCat->QueueAction(new PlayAnimationAction(parentCat, "Jumping Start"));
	parentCat->QueueAction(new JumpingAction(parentCat, window));
	parentCat->QueueAction(new PlayAnimationAction(parentCat, "Jumping Middle"));
	JumpingAction::QueueJumpWaitFunction(parentCat, window);
	parentCat->QueueAction(new PlayAnimationAction(parentCat, "Jumping End"));
}

void JumpingAction::Start()
{
	RECT bounds;

	if (Window != nullptr && CWindowsManager::GetInstance().GetWindowBoundry(Window, bounds))
	{
		JumpHeight = (ParentCat->Location.y - bounds.top);

		if (JumpHeight > ParentCat->JumpHeight ||
			JumpHeight <= 0.0f)
		{
			ParentCat->ClearSubsequentActions();
			return;
		}
		else
		{
			JumpHeight += OverJumpOffset;
		}
	}

	TargetY = ParentCat->Location.y - JumpHeight;
	ParentCat->Velocity.y = -sqrtf(2 * ParentCat->Acceleration.y * JumpHeight);
	ParentCat->SetCurrentState(CatState::Jumping);
}

CarriedAction::CarriedAction(CCat* parentCat) : MM(CMonitorManager::GetInstance()),
Action(parentCat)
{
}

CarriedAction::~CarriedAction()
{
	ParentCat->RotationZ = 0.0f;
}

void CarriedAction::Start()
{
	ParentCat->SetCurrentState(CatState::Carried);
	ParentCat->SetAnimationIndex("Carried");
}

bool CarriedAction::Update()
{
	glm::vec3 mouseLoc = glm::vec3(GINPUT::GetMouseLocation(), 0.0f);
	ParentCat->Location = mouseLoc;
	
	UpdateRotation();

	InputState lbuttonState = GINPUT::GetLMButtonState();
	if (lbuttonState == InputState::INPUT_UP_FIRST || lbuttonState == InputState::INPUT_UP)
	{
		glm::vec2 deltaLoc = GINPUT::GetDeltaMouseLocation();
		float deltaTime = TIMER::GetDeltaSeconds();

		ParentCat->Velocity = deltaLoc / deltaTime;
		ParentCat->QueueAction(new BounceAction(ParentCat));

		//float strength = glm::length(ParentCat->Velocity);

		//if (strength > 5.0f)
		//{
		//	ParentCat->QueueAction(new BounceAction(ParentCat));
		//}
		//else
		//{
		//	ParentCat->QueueAction(new FallingAction(ParentCat));
		//}

		return true;
	}

	return false;
}

void CarriedAction::UpdateRotation()
{
	glm::vec2 deltaLoc = GINPUT::GetDeltaMouseLocation();
	float dt = TIMER::GetDeltaSeconds();

	RotationalVelocity += deltaLoc.x * MovementMultiplier * dt;
	RotationalVelocity -= ParentCat->RotationZ * PendulumMultiplier * dt;

	if (HELPER::Sign(RotationalVelocity) != HELPER::Sign(ParentCat->RotationZ))
	{ RotationalVelocity *= (1.0f - dt * GravitationalMultiplier); }

	ParentCat->RotationZ += RotationalVelocity * dt;
	ParentCat->RotationZ = std::clamp(ParentCat->RotationZ, -90.0f, 90.0f);
}

FallingAction::FallingAction(CCat* parentCat) :
	Action(parentCat)
{
	OldVelocity = ParentCat->Velocity.y;
	ParentCat->SetCurrentState(CatState::Falling);
	ParentCat->SetAnimationIndex("Falling");
}

FallingAction::~FallingAction()
{
	ParentCat->SetAnimationIndex("Standing");
	ParentCat->SetCurrentState(CatState::Sliding);
	ParentCat->QueueAction(new WaitAction(ParentCat, 1.0f));
}

bool FallingAction::Update()
{
	bool val = ParentCat->Velocity.y == 0.0f && OldVelocity == 0.0f;
	OldVelocity = ParentCat->Velocity.y;
	return val;
}

BounceAction::BounceAction(CCat* parentCat) :
	Action(parentCat)
{
	OldVelocity = glm::vec2();
}

BounceAction::~BounceAction()
{
	// Clear any rotation
	ParentCat->RotationZ = 0.0f;
	ParentCat->Offset *= 0.0f;
	ParentCat->SetCurrentState(CatState::Sliding);
}

void BounceAction::Start()
{
	ParentCat->SetCurrentState(CatState::Falling);
	ParentCat->SetAnimationIndex("Bouncing");
	ParentCat->QueueAction(new WaitAction(ParentCat, 2.0f));

	OldVelocity = ParentCat->Velocity;
	ParentCat->Offset.y = ParentCat->GetImageHeight() * ParentCat->Scale.y * -0.5f;
}

bool BounceAction::Update()
{
	glm::vec2 absOld = glm::abs(OldVelocity);

	// Reflect velocity
	if (ParentCat->Velocity.x == 0 && absOld.x > 1.0f) { ParentCat->Velocity.x = -OldVelocity.x * 0.5f; }
	if (ParentCat->Velocity.y == 0 && absOld.y > 1.0f) { ParentCat->Velocity.y = -OldVelocity.y * 0.5f; }

	// Check whether cat has stopped moving
	bool val = ParentCat->Velocity == glm::zero<glm::vec2>() && OldVelocity == glm::zero<glm::vec2>();
	OldVelocity = ParentCat->Velocity;

	// Cat starts sliding once there is no more bouncing
	if (ParentCat->Velocity.y == 0 && OldVelocity.y == 0)
	{
		std::string current = ParentCat->GetAnimationIndex();
		std::string sliding = "Sliding " + (std::string)(ParentCat->Velocity.x > 0.0f ? "Left" : "Right");
		bool isSliding = current.rfind("Sliding", 0) == 0;
		bool isFastEnough = std::abs(ParentCat->Velocity.x) > 50.0f;

		ParentCat->SetAnimationIndex(isSliding ? (isFastEnough ? sliding : current) : sliding);

		// Clear transforms
		ParentCat->RotationZ = 0;
		ParentCat->Offset *= 0.0f;
	}
	else if (!val)
	{
		ParentCat->RotationZ += 600.0f * TIMER::GetDeltaSeconds() * (ParentCat->Velocity.x > 0.0f ? 1.0f : -1.0f);
		ParentCat->Offset.y = ParentCat->GetImageHeight() * ParentCat->Scale.y * -0.5f;
		ParentCat->SetAnimationIndex("Bouncing");
	}

	return val;
}

WaitAction::WaitAction(CCat* parentCat, float seconds) :
	Action(parentCat)
{
	Seconds = seconds;
}

WaitAction::WaitAction(CCat* parentCat, std::function<bool()> function) :
	Action(parentCat)
{
	Seconds = 0.0f;
	Function = function;
}

void WaitAction::Start()
{
	StartTime = std::chrono::steady_clock::now();
}

bool WaitAction::Update()
{
	if (Function != NULL)
	{
		return Function();
	}
	else
	{
		std::chrono::duration<float> tempTotalSec = std::chrono::steady_clock::now() - StartTime;
		float duration = tempTotalSec.count();
		return duration >= Seconds;
	}
}

PlayAnimationAction::PlayAnimationAction(CCat* parentCat, std::string animationID) :
	Action(parentCat)
{
	IsFinished = parentCat->GetAnimation(animationID).AnimationType == EAnimationType::Loop;
	AnimationID = animationID;
}

void PlayAnimationAction::Start()
{
	ParentCat->SetAnimationIndex(AnimationID);
}

bool PlayAnimationAction::Update()
{
	return IsFinished;
}

void PlayAnimationAction::OnAnimationEnd()
{
	IsFinished = true;
}

SittingAction::SittingAction(CCat* parentCat) : Action(parentCat) { }

void SittingAction::Start()
{
	ParentCat->SetAnimationIndex("Idle");
	ParentCat->SetCurrentState(CatState::Sitting);
}

void SittingAction::QueueSittingActions(CCat* parentCat, float seconds)
{
	// Pregenerate the random device and get the idle animations
	std::vector<std::pair<std::string, float>> idleAnimations = parentCat->GetIdleAnimations();
	auto rd = std::random_device{};
	auto rng = std::default_random_engine{ rd() };

	float leftOverTime = seconds;

	// Continue until all the time is filled up
	while (leftOverTime > 0.001f)
	{
		// Generate a time to hold the neutral pose
		float waitTime = std::min(leftOverTime, HELPER::Random(5.0f, 10.0f));
		leftOverTime -= waitTime;

		// Queue on a sitting/wait animation
		parentCat->QueueAction(new SittingAction(parentCat));
		parentCat->QueueAction(new WaitAction(parentCat, waitTime));

		if (leftOverTime > 0.001f)
		{
			// Shuffle the idle animations so that a different one is played
			std::shuffle(std::begin(idleAnimations), std::end(idleAnimations), rng);

			// Loop through and find an animation that fits within the remaining time
			for (std::pair<std::string, float> animation : idleAnimations)
			{
				if (animation.second <= leftOverTime)
				{
					// Calculate the new left over time and play the animation
					parentCat->QueueAction(new PlayAnimationAction(parentCat, animation.first));
					leftOverTime -= animation.second;
					break;
				}
			}
		}
	}
}

FunctionAction::FunctionAction(std::function<void()> aFunction) :
	Action(nullptr)
{
	Function = aFunction;
}

void FunctionAction::Start()
{
	Function();
}

WalkToWindowAction::WalkToWindowAction(CCat* parentCat, HWND targetWindow) :
	Action(parentCat),
	WM(CWindowsManager::GetInstance())
{
	TargetWindow = targetWindow;
}

WalkToWindowAction::~WalkToWindowAction()
{
	ParentCat->Velocity.x = 0.0f;
}

void WalkToWindowAction::Start()
{
	ParentCat->SetCurrentState(CatState::Walking);
	ParentCat->SetAnimationIndex("Idle");
	ParentCat->Velocity.x = 1.0f;
	Update();
	if (ParentCat->Velocity.x == 0.0f) { ParentCat->Velocity.x = 1.0f; }
}

bool WalkToWindowAction::Update()
{
	// Retrieve the bounds of the target window
	std::vector<Platform> platforms;
	if (WM.GetPlatformsFromWindow(TargetWindow, platforms))
	{
		// Window exists:
		
		// Check if window is still within jumping distance
		float dist = ParentCat->Location.y - platforms[0].Height;
		if (dist <= 0 || dist > ParentCat->JumpHeight)
		{
			// Cat cannot jump to the window anymore
			ParentCat->ClearSubsequentActions();
			return true;
		}

		float targetX = 0.0f;
		float distance = INT_MAX;

		for (Platform p : platforms)
		{
			float value = std::clamp(ParentCat->Location.x, p.Left, p.Right);
			float delta = std::abs(ParentCat->Location.x - value);

			if (delta < distance)
			{
				distance = delta;
				targetX = value;
			}
		}
		
		// Get the boundry of any window the cat is currently standing on
		RECT currentWindBounds;
		if (WM.GetWindowBoundry(ParentCat->GetCurrentWindow(), currentWindBounds))
		{
			// Check whether the cat has a platform to stand on
			if (targetX <= currentWindBounds.left ||
				targetX >= currentWindBounds.right)
			{
				// Cat has no spot to stand on
				ParentCat->ClearSubsequentActions();
				return true;
			}
		}

		// Get current and target directions
		DIRECTION walkDirection = targetX >= ParentCat->Location.x ? DIRECTION::RIGHT : DIRECTION::LEFT;
		std::string targetAnimationID = (walkDirection == DIRECTION::LEFT ? "Walking Left" : "Walking Right");
		std::string currentAnimationID = ParentCat->GetAnimationIndex();

		// Only update direction if required
		if (targetAnimationID != currentAnimationID)
		{
			ParentCat->Velocity.x = (float)walkDirection * ParentCat->WalkSpeed;
			ParentCat->SetAnimationIndex(targetAnimationID);
		}


		float delta= std::fabsf(ParentCat->Location.x - targetX);

		if (delta <= 2.0f)
		{
			ParentCat->Location.x = targetX;
			ParentCat->Velocity.x = 0.0f;
			return true;
		}
		// Check if the cat has walked into something
		else if (ParentCat->Velocity.x == 0.0f)
		{
			ParentCat->ClearSubsequentActions();
			return true;
		}

		// Target not reached yet
		return false;
	}
	else
	{
		// Window does not exist stop all queued actions
		ParentCat->ClearSubsequentActions();
		return true;
	}
}