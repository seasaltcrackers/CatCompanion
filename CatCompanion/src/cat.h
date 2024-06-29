#pragma once
#include <Windows.h>
#include <queue>
#include <list>
#include <chrono>
#include <wtypes.h>

#include "sprite.h"

class CFont;
class CScene;
class CCatManager;
class CMonitorManager;
class CWindowsManager;
class CFocusManager;
class CConfigs;

struct Action;

enum class DIRECTION
{
	LEFT = -1,
	RIGHT = 1,
};


enum class CatState
{
	Sitting,
	Sleeping,
	Walking,
	Jumping,
	Falling,
	Carried,
	Sliding,
	IconExpanding,
	IconFalling,
	IconEating,
	IconProcessing,
};

class CCat :
	public CSprite
{
public:
	CCat(CCatManager* parent, CConfigs* configs, glm::vec2 location);
	virtual ~CCat();

	void Update() override;
	void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;
	void OnFileDropped(HICON icon);

	void UpdateMouseEvents();
	void ProcessActions();
	void UpdateActions();
	void UpdatePhysics();
	void ConfineToMonitors();
	void WindowCollision();
	void UpdateIcon();

	void OnLMBDown();
	void OnRMBDown();
	void OnLMBUp();
	void OnRMBUp();
	void OnRMBHeld();
	void OnRMBPressed();

	float GetBoundsWidth();
	float GetBoundsHeight();
	bool IsInBounds(glm::vec2 location);

	int GetNumberOfColours();
	glm::vec3 GetColour(int index);
	void SetColour(int index, glm::vec3 colour);

	CConfigs* GetConfigs();
	void SetListIterator(std::list<CCat*>::iterator listIt);

	void SetCurrentState(CatState newState);
	CatState GetCurrentState();

	void ClearActions();
	void ClearSubsequentActions();
	void QueueAction(Action* newAction);

	bool TryJumpToWindow();
	std::vector<std::pair<std::string, float>> GetIdleAnimations();
	bool IsLocationValid(glm::vec3 location);
	HWND GetCurrentWindow();
	
	void SetName(std::string name);
	std::string GetName();

	void Duplicate();
	void Delete();

	glm::vec2 Velocity;
	glm::vec2 Acceleration;
	glm::vec3 Offset;

	float WalkSpeed = 35.0f;
	float JumpHeight = 400.0f;
	
private:
	CMonitorManager& MM;
	CWindowsManager& WM;
	CFocusManager& FM;

	CCatManager* ParentManager;
	CScene* Parent;
	CConfigs* Configs;
	std::list<CCat*>::iterator ListIt;

	std::string Name;
	glm::ivec2 NameDimensions;
	CFont* NameFont;
	float NameAlpha;
	const float NameAlphaRate = 2.0f;


	CObject* IconObject;
	GLuint IconTexture;
	glm::vec3 IconVelocity;

	float FloorY;
	glm::vec3 OldLoc;
	HWND CurrentWindow;

	CatState CurrentState;

	Action* CurrentAction;
	std::queue<Action*> Actions;

	glm::vec3 Colours[255];
	int NumberOfColours;

	std::chrono::steady_clock::time_point RClickTimePoint;
	bool RButtonDown;

	RECT GetBoundsWind();
	RECT GetBoundsDesk();
	
	void UpdateColours();
	void GenerateAnimations() override;
	void UpdateModelMatrix() override;
	void OnAnimationEnd() override;

	GLuint HiconToTexture(HICON hIcon);

	void CalculateHues();

	bool IsSpawningHearts;
	float SpawningHeartInterval = 0.1f;

	HCURSOR PattingCursor;

	static bool HasLoadedHues;

	void SpawnHeart();
};