#pragma once
#include <functional>
#include <windows.h>
#include <chrono>
#include <glm.hpp>

class CCat;
class CWindowsManager;
class CMonitorManager;
enum class DIRECTION;

struct Action
{
public:
	Action(CCat* parentCat) : ParentCat(parentCat) { }
	virtual ~Action() { }

	virtual void Start() { } // Executes at the start of the action
	virtual bool Update() { return true; } // Returns true when action has completed
	virtual void OnAnimationEnd() {}; // Executes whenever an animation finishes

protected:
	CCat* ParentCat;
};

struct FunctionAction :
	Action
{
public:
	FunctionAction(std::function<void()> aFunction);

private:
	void Start() override;
	std::function<void()> Function;
};

struct PlayAnimationAction :
	Action
{
public:
	PlayAnimationAction(CCat* parentCat, std::string animationID);

	void Start() override;
	bool Update() override;
	void OnAnimationEnd() override;

private:
	std::string AnimationID;
	bool IsFinished;
};

struct WalkToWindowAction :
	Action
{
public:
	WalkToWindowAction(CCat* parentCat, HWND targetWindow);
	~WalkToWindowAction();

	void Start() override;
	bool Update() override;

private:
	HWND TargetWindow;
	CWindowsManager& WM;
};

struct WalkTargetAction :
	Action
{
public:
	WalkTargetAction(CCat* parentCat, float targetX);
	~WalkTargetAction();

	void Start() override;
	bool Update() override;

private:
	DIRECTION WalkDirection;
	float TargetX;
};

struct WalkTimedAction :
	Action
{
public:
	WalkTimedAction(CCat* parentCat, DIRECTION direction, float seconds);
	~WalkTimedAction();

	void Start() override;
	bool Update() override;

private:
	DIRECTION WalkDirection;
	float Seconds;
	std::chrono::steady_clock::time_point StartTime;
};

struct WaitAction :
	Action
{
public:
	WaitAction(CCat* parentCat, float seconds);
	WaitAction(CCat* parentCat, std::function<bool()> function);

	void Start() override;
	bool Update() override;

private:
	float Seconds;
	std::chrono::steady_clock::time_point StartTime;
	std::function<bool()> Function;
};

struct SittingAction :
	Action
{
public:
	void Start() override;
	static void QueueSittingActions(CCat* parentCat, float seconds);

private:
	SittingAction(CCat* parentCat);
};

struct SleepingAction :
	Action
{
public:
	SleepingAction(CCat* parentCat, float seconds);

	void Start() override;
	bool Update() override;

private:
	float Seconds;
	std::chrono::steady_clock::time_point StartTime;
};

struct CarriedAction :
	Action
{
public:
	CarriedAction(CCat* parentCat);
	virtual ~CarriedAction();

	void Start() override;
	bool Update() override;

private:
	void UpdateRotation();

	CMonitorManager& MM;

	float RotationalVelocity = 0.0f;
	float GravitationalMultiplier = 10.0f;
	float PendulumMultiplier = 100.0f;
	float MovementMultiplier = 70.0f;
};

struct BounceAction :
	Action
{
public:
	BounceAction(CCat* parentCat);
	virtual ~BounceAction();

	void Start() override;
	bool Update() override;

private:
	glm::vec2 OldVelocity;
};

struct FallingAction :
	Action
{
public:
	FallingAction(CCat* parentCat);
	virtual ~FallingAction();

	bool Update() override;

private:
	float OldVelocity;
};

struct JumpingAction :
	Action
{
public:
	static void QueueJumpingActions(CCat* parentCat, float height);
	static void QueueJumpingActions(CCat* parentCat, HWND window);

	void Start() override;

private:
	JumpingAction(CCat* parentCat, float height);
	JumpingAction(CCat* parentCat, HWND window);

	static void QueueJumpWaitFunction(CCat* parentCat, HWND window = NULL, float height = -1.0f);

	float JumpHeight;
	float TargetY;
	HWND Window;

	static const inline float JumpStartDistance = 8.0f;
	static const inline float JumpStartFrames = 6.0f;
	static const inline float OverJumpOffset = 35.0f;
};