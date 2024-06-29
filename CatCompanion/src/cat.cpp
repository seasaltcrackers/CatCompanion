#include <random>
#include <math.h>
#include <numeric>
#include <set>
#include <SOIL.h>
#include <cmath>

#include "cat.h"

#include "timer.h"
#include "input.h"
#include "helper.h"
#include "font.h"
#include "graphics.h"
#include "mesh.h"
#include "program.h"

#include "scene.h"
#include "actions.h"
#include "coreTilemap.h"
#include "monitorManager.h"
#include "windowsManager.h"
#include "focusManager.h"
#include "configs.h"
#include "catHelper.h"
#include "texturedParticleLayer.h"
#include "catManager.h"

bool CCat::HasLoadedHues = false;

CCat::CCat(CCatManager* parent, CConfigs* configs, glm::vec2 location) :
	MM(CMonitorManager::GetInstance()),
	WM(CWindowsManager::GetInstance()),
	FM(CFocusManager::GetInstance()),
	CSprite()
{
	ParentManager = parent;
	Configs = configs;

	// Update default colours
	std::fill(std::begin(Colours), std::end(Colours), glm::vec3());
	Colours[0] = glm::vec3(1.0f, 1.0f, 1.0f);

	Program = CProgram::GetProgram("Cat");
	UpdateUniformfVec3Array("lookup", 100, glm::value_ptr(Colours[0]));

	NumberOfColours = parent->NumberOfColours;

	GenerateAnimations();

	for (int i = 1; i <= NumberOfColours; ++i)
	{
		std::string name = "colours[" + std::to_string(i) + "]";
		if (Configs->DoesStringExist(name))
		{
			std::string hex = Configs->GetStringVariable(name);
			
			if (hex.substr(hex.length() - 1, 1) == "!")
			{
				hex = hex.substr(0, hex.length() - 1);
				Colours[i] = HELPER::HEXtoRGB(hex);
				Colours[i] /= -255.0f;
			}
			else
			{
				Colours[i] = HELPER::HEXtoRGB(hex);
				Colours[i] /= 255.0f;
			}
		}
	}

	PattingCursor = LoadCursorFromFile(L"Resources\\Other\\Hand.cur");

	NameFont = CFont::GetFont("AcknowledgeTTSmall");
	std::string name = Configs->GetStringVariable("name");
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	SetName(name);

	float scale = Configs->GetFloatVariable("scale");

	Offset = glm::vec3();
	Location = glm::vec3(location, 0.0f);
	Scale = glm::vec3(scale, scale, 1.0f);
	Acceleration = glm::vec2(0.0f, 5000.0f);
	FloorY = Location.y;

	WalkSpeed = Configs->GetFloatVariable("walk-speed");
	JumpHeight = Configs->GetFloatVariable("max-jump-height");

	CurrentAction = nullptr;
	CurrentState = CatState::Sitting;

	IconObject = nullptr;
	Parent = parent->Parent;

	TexturedParticleType heartParticles;
	heartParticles.LocationVariation = 4.0f;
	heartParticles.VelocityVariation = 30.0f;
	heartParticles.LifeTimeCenter = 2.0f;
	heartParticles.ScaleCenter = 18.0f;
	heartParticles.AccelerationCenter = glm::vec2(0.0f, -100.0f);
	heartParticles.AccelerationVariation = 50.0f;
	heartParticles.TextureID = CTexture::GetTexture("Heart")->GetID();

	Parent->ParticlesLayer->AddParticleType("Hearts", heartParticles);

	IsSpawningHearts = false;

	SittingAction::QueueSittingActions(this, HELPER::Random(2.0f, 5.0f));
}

CCat::~CCat()
{
	delete Configs;
	Configs = nullptr;

	if (IconObject != nullptr)
	{
		delete IconObject;
		IconObject = nullptr;
	}

	ClearActions();
}

void CCat::Update()
{
	OldLoc = Location;

	UpdateColours();
	UpdateMouseEvents();
	ProcessActions();
	UpdatePhysics();
	UpdateIcon();

	CSprite::Update();
}

void CCat::Draw(glm::mat4& pv)
{
	CSprite::Draw(pv);

	if (IconObject != nullptr)
	{
		IconObject->Draw(pv);
	}

	if (NameAlpha > 0.0f && Name != "")
	{
		//glm::ivec2 location = glm::ivec2(Location.x - NameDimensions.x * 0.5f, Location.y - GetImageHeight() * CScene::TileScaler - 25.0f);
		glm::ivec2 location = glm::ivec2(Location.x - NameDimensions.x * 0.5f, Location.y - 28.0f * Scale.x);
		GFX::DrawString(pv, NameFont, Name, location, glm::vec4(1.0f, 1.0f, 1.0f, NameAlpha));
	}
}

void CCat::OnFileDropped(HICON icon)
{
	if ((CurrentState == CatState::Sitting ||
		 CurrentState == CatState::Sleeping ||
		 CurrentState == CatState::Walking) &&
		CurrentState != CatState::IconEating &&
		CurrentState != CatState::IconProcessing &&
		CurrentState != CatState::IconExpanding &&
		CurrentState != CatState::IconFalling &&
		IconObject == nullptr)
	{
		IconTexture = HiconToTexture(icon);
		std::vector<int> hues;
		HELPER::GetTopHues(icon, hues);

		for (int i = 0; i < hues.size(); ++i)
		{
			HELPER::PrintToOutput(std::to_string(i) + ":" + std::to_string(hues[i]) + ", ");
		}

		IconObject = new CObject(CProgram::GetProgram("Texture"), CMesh::GetMesh("2D Square"));
		IconObject->UpdateUniformTexture("tex", IconTexture);

		IconObject->Scale *= 1.0f;
		IconObject->Scale.y *= -1.0f;
		IconObject->Location = Location;
		IconObject->Pivot = glm::vec3(0.5f, 0.5f, 0.0f);

		IconVelocity = glm::vec3(0.0f, -375.0f, 0.0f);

		CurrentState = CatState::IconExpanding;

		ClearActions();
		QueueAction(new PlayAnimationAction(this, "Idle"));
		QueueAction(new WaitAction(this, 0.75f));
		QueueAction(new FunctionAction([this]() { CurrentState = CatState::IconFalling; }));
		QueueAction(new PlayAnimationAction(this, "Eating Icon"));
		QueueAction(new FunctionAction([this]() { CurrentState = CatState::IconProcessing; }));
		QueueAction(new WaitAction(this, 0.75f));
	}
}


void CCat::OnLMBDown()
{
	if (CurrentState != CatState::Carried)
	{
		ClearActions();
		CurrentWindow = NULL;
		Actions.push(new CarriedAction(this));
	}
}

void CCat::OnRMBDown() { }

void CCat::OnLMBUp() { }

void CCat::OnRMBUp()
{
	if (IsSpawningHearts)
	{
		IsSpawningHearts = false;

		HCURSOR normalCursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(normalCursor);
		SetClassLongPtr(CScene::hWnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(normalCursor));
	}
}

void CCat::OnRMBHeld()
{
	SetCursor(PattingCursor);
	SetClassLongPtr(CScene::hWnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(PattingCursor));

	IsSpawningHearts = true;
	SpawnHeart();
}

void CCat::OnRMBPressed()
{
	Parent->OpenCatPropertiesWindow(this);
}


void CCat::UpdateMouseEvents()
{
	if (CurrentState != CatState::Carried)
	{
		bool inBounds = IsInBounds(GINPUT::GetMouseLocation());

		if (inBounds)
		{
			FM.SetFocused();

			NameAlpha += NameAlphaRate * TIMER::GetDeltaSeconds();
			NameAlpha = std::min(1.0f, NameAlpha);
		}
		else
		{
			NameAlpha -= NameAlphaRate * TIMER::GetDeltaSeconds();
			NameAlpha = std::max(0.0f, NameAlpha);
		}
	}
	else
	{
		NameAlpha += NameAlphaRate * TIMER::GetDeltaSeconds();
		NameAlpha = std::min(1.0f, NameAlpha);
	}

	if (IsSpawningHearts)
	{
		FM.SetFocused();
	}
}

void CCat::ProcessActions()
{
	if (std::abs(Velocity.y) > 0 && CurrentState != CatState::Falling && CurrentState != CatState::Jumping)
	{
		ClearActions();
		QueueAction(new BounceAction(this));
	}

	if (CurrentAction != nullptr)
	{
		bool result = CurrentAction->Update();

		if (result)
		{
			delete CurrentAction;
			CurrentAction = nullptr;

			if (!Actions.empty())
			{
				CurrentAction = Actions.front();
				Actions.pop();

				CurrentAction->Start();
			}
		}
	}
	else if (!Actions.empty())
	{
		CurrentAction = Actions.front();
		Actions.pop();

		CurrentAction->Start();
	}
	else
	{
		UpdateActions();
	}
}

void CCat::UpdateActions()
{
	if (CurrentState != CatState::Sitting)
	{
		SittingAction::QueueSittingActions(this, HELPER::Random(10.0f, 60.0f));
	}
	else
	{
		// 70% chance to try jump to a window
		if (HELPER::Random(0.0f, 100.0f) <= 70.0f && TryJumpToWindow())
		{
			// TryJumpToWindow adds it's actions so no need to continue
			return;
		}

		// 30% chance to walk around
		RECT windowBounds;
		if (CurrentWindow != NULL && WM.GetWindowBoundry(CurrentWindow, windowBounds))
		{
			// Calculate a random target point
			float halfWidth = GetBoundsWidth() * 0.5f;
			float targetX = HELPER::Random(windowBounds.left + halfWidth, windowBounds.right - halfWidth);
			float delta = targetX - Location.x;
			float absolute = std::abs(delta);
			float sign = HELPER::Sign(delta);
			sign = (sign == 0 ? 1 : sign);

			// Lock the target position to a minimum and maximum distance
			absolute = std::clamp(absolute, 100.0f, 400.0f);
			targetX = Location.x + absolute * sign;

			// Walk to that point
			Actions.push(new WalkTargetAction(this, targetX));
		}
		else
		{
			// Walk in a random direction for 4-10 seconds
			Actions.push(new WalkTimedAction(this, (DIRECTION)((rand() % 2) * 2 - 1), HELPER::Random(4.0f, 10.0f)));
		}

		// 10% chance to yawn and fall asleep for 5-10minutes after walking
		if (HELPER::Random(0.0f, 100.0f) <= 10.0f)
		{
			Actions.push(new FunctionAction([this]() { SetCurrentState(CatState::Sitting); }));
			Actions.push(new PlayAnimationAction(this, "Idle Yawn"));
			Actions.push(new SleepingAction(this, HELPER::Random(300.0f, 600.0f)));
			Actions.push(new PlayAnimationAction(this, "Idle Yawn"));
		}
	}
}

void CCat::UpdatePhysics()
{
	Velocity += Acceleration * TIMER::GetDeltaSeconds();

	if (CurrentState == CatState::Carried)
	{
		Velocity.y = 0.0f;
		Location += glm::vec3(Velocity * TIMER::GetDeltaSeconds(), 0.0f);
	}
	else
	{
		Location += glm::vec3(Velocity * TIMER::GetDeltaSeconds(), 0.0f);

		glm::vec3 oldLocation = Location;
		WindowCollision();
		ConfineToMonitors();

		glm::vec3 locA = HELPER::RoundToPlaces(Location, 1);
		glm::vec3 locB = HELPER::RoundToPlaces(oldLocation, 1);

		Velocity *= glm::vec2(1.0f - glm::abs(glm::sign(locA - locB)));

		if (Velocity.y == 0.0f && CurrentState != CatState::Walking)
		{
			HELPER::AddFrictionForce(Velocity, 2000.0f);
		}
	}
}

void CCat::ConfineToMonitors()
{
	// Offset to the top left and right position
	const glm::vec2 topLeftOffset = glm::vec2(GetBoundsWidth() * 0.5f, GetBoundsHeight());
	const glm::vec2 topRightOffset = glm::vec2(-GetBoundsWidth() * 0.5f, 0.0f);

	// All monitors
	std::vector<HMONITOR> monitors = MM.GetMonitors();

	// Used to find the shortest distance to move
	float length = (float)INT_MAX;
	glm::vec2 confinedLocation;

	// Current location
	glm::vec2 oldLocation = MM.WindowToDesktop(glm::vec2(Location));

	// Collection of intersections with the monitors
	std::vector<RECT> intersections;

	// Get player boundry
	RECT bounds = GetBoundsDesk();

	// Calculate the players dimensions
	float playerWidth = bounds.right - bounds.left;
	float playerHeight = bounds.bottom - bounds.top;

	// Loop through each monitor
	for (int i = 0; i < (int)monitors.size(); ++i)
	{
		// Retrieve the data of the current monitor
		int monitor = (int)monitors[i];
		MonitorInfo& info = MM.GetMonitorDetail(monitor);

		// Get the intersection rectangle between the monitor and the cat
		RECT tempIntersection;
		IntersectRect(&tempIntersection, &info.BoundryTB, &bounds);
		intersections.push_back(tempIntersection);

		// Create a location that solves the conflict
		glm::vec2 newLocation = glm::clamp(oldLocation, glm::vec2{ info.BoundryTB.left, info.BoundryTB.top } + topLeftOffset, glm::vec2{ info.BoundryTB.right, info.BoundryTB.bottom } + topRightOffset);
		float deltaLength = glm::length(newLocation - oldLocation);

		// Update the confined location if the difference from the old location is less than the previous solution
		if (deltaLength < length)
		{
			length = deltaLength;
			confinedLocation = newLocation;

			// If the conflicted solution distance is 0 then exit out
			if (length == 0) { return; }

			glm::vec2 oldLocation2 = MM.WindowToDesktop(glm::vec2(Location));
			int i = 0;
		}
	}

	// Split the collision into 4 sections (this is because anymore is unlikely to happen in terms of monitors)
	float topLeftW = 0.0f;
	float topLeftH = 0.0f;

	float topRightW = 0.0f;
	float topRightH = 0.0f;

	float botLeftW = 0.0f;
	float botLeftH = 0.0f;

	float botRightW = 0.0f;
	float botRightH = 0.0f;

	// Loop through each intersection
	for (int i = 0; i < intersections.size(); ++i)
	{
		// Retrieve the data of the current intersection
		RECT intersec = intersections[i];
		float width = intersec.right - intersec.left;
		float height = intersec.bottom - intersec.top;

		// Check where the intersection takes place
		bool left = bounds.left == intersec.left;
		bool right = bounds.right == intersec.right;
		bool top = bounds.top == intersec.top;
		bool bottom = bounds.bottom == intersec.bottom;

		// Update the corrosponding segments

		if (left && right)
		{
			if (top)
			{
				topLeftW = width;
				topRightH = playerWidth - width;

				topLeftH = height;
				topRightH = height;
			}
			else
			{
				botLeftW = width;
				botRightW = playerWidth - width;

				botLeftH = height;
				botRightH = height;
			}
		}
		else if (top && bottom)
		{
			if (left)
			{
				topLeftW = width;
				botLeftW = width;

				topLeftH = height;
				botLeftH = playerHeight - height;
			}
			else
			{
				topRightW = width;
				botRightW = width;

				topRightH = height;
				botRightH = playerHeight - height;
			}
		}
		else
		{
			if (left)
			{
				if (top)
				{
					topLeftW = width;
					topLeftH = height;
				}
				else if (bottom)
				{
					botLeftW = width;
					botLeftH = height;
				}
			}

			if (right)
			{
				if (top)
				{
					topRightW = width;
					topRightH = height;
				}
				else if (bottom)
				{
					botRightW = width;
					botRightH = height;
				}
			}
		}
	}

	// Retrieve the combined width and height of the intersections
	float combinedWidth = std::min(topLeftW + topRightW, botLeftW + botRightW);
	float combinedHeight = std::min(topLeftH + botLeftH, topRightH + botRightH);

	// Check for a difference between the intersection and the player dimensions
	if (combinedWidth != playerWidth ||
		combinedHeight != playerHeight)
	{
		glm::vec2 currentDesktopLocation = MM.WindowToDesktop(glm::vec2(Location));

		if (topLeftH == topRightH && topRightH != 0.0f)
		{
			glm::vec2 newLocation = glm::vec2(currentDesktopLocation.x, std::ceilf(currentDesktopLocation.y - (playerHeight - topLeftH)));
			float deltaLength = glm::length(newLocation - oldLocation);

			// Update the confined location if the difference from the old location is less than the previous solution
			if (deltaLength < length)
			{
				length = deltaLength;
				confinedLocation = newLocation;
			}
		}


		if (botLeftH == botRightH && botRightH != 0.0f)
		{
			glm::vec2 newLocation = glm::vec2(currentDesktopLocation.x, std::floorf(currentDesktopLocation.y + (playerHeight - botLeftH)));
			float deltaLength = glm::length(newLocation - oldLocation);

			// Update the confined location if the difference from the old location is less than the previous solution
			if (deltaLength < length)
			{
				length = deltaLength;
				confinedLocation = newLocation;
			}
		}

		if (botLeftW == topLeftW && topLeftW != 0.0f)
		{
			glm::vec2 newLocation = glm::vec2(std::ceilf(currentDesktopLocation.x - (playerWidth - botLeftW)), currentDesktopLocation.y);
			float deltaLength = glm::length(newLocation - oldLocation);

			// Update the confined location if the difference from the old location is less than the previous solution
			if (deltaLength < length)
			{
				length = deltaLength;
				confinedLocation = newLocation;
			}
		}


		if (botRightW == topRightW && topRightW != 0.0f)
		{
			glm::vec2 newLocation = glm::vec2(std::floorf(currentDesktopLocation.x + (playerWidth - botRightW)), currentDesktopLocation.y);
			float deltaLength = glm::length(newLocation - oldLocation);

			// Update the confined location if the difference from the old location is less than the previous solution
			if (deltaLength < length)
			{
				length = deltaLength;
				confinedLocation = newLocation;
			}
		}

		// Move the player back to it's old location
		Location = glm::vec3(MM.DesktopToWindow(confinedLocation), 0.0f);
		CurrentWindow = NULL;
	}
}

void CCat::WindowCollision()
{
	float halfPlayerWidth = GetBoundsWidth() * 0.5f;
	float playerHeight = GetBoundsHeight();

	HWND newWind = NULL;

	// Only collide with windows if the cat is traveling downwards
	if (Velocity.y > 0)
	{
		// Get all non maximised windows
		std::vector<Platform>& platforms = WM.GetPlatforms();

		// Check for collision
		for (Platform& p : platforms)
		{
			if (p.Hwnd == CurrentWindow)
			{
				if (Location.x + halfPlayerWidth > p.Left && Location.x - halfPlayerWidth < p.Right && // Within line segment
					Location.y >= p.Height)  // Let the player drop if the window moves down
				{
					newWind = CurrentWindow;
					Location.y = p.Height;
				}
				else
				{
					CurrentWindow = NULL;
				}
			}
			else
			{
				if (OldLoc.y <= p.Height && Location.y > p.Height) // Check if player has tried to travel through the window
				{
					if (Location.x + halfPlayerWidth > p.Left && Location.x - halfPlayerWidth < p.Right) // Within line segment
					{
						// Lock to the top of the window
						Location.y = p.Height;
						Velocity.y = 0.0f;
						newWind = p.Hwnd;
					}
				}
			}
		}

		CurrentWindow = newWind;
	}
}

void CCat::UpdateIcon()
{
	if (IconObject != nullptr)
	{
		switch (CurrentState)
		{
			case CatState::IconExpanding:
			{
				IconVelocity += glm::vec3(0.0f, 600.0f, 0.0f) * TIMER::GetDeltaSeconds();

				float deltaScale = 32.0f - IconObject->Scale.x;
				IconObject->Scale *= 1.0f + (deltaScale * TIMER::GetDeltaSeconds());
				IconObject->Location += IconVelocity * TIMER::GetDeltaSeconds();
				break;
			}

			case CatState::IconFalling:
			{
				IconVelocity += glm::vec3(0.0f, 600.0f, 0.0f) * TIMER::GetDeltaSeconds();

				glm::vec3 delta = IconObject->Location - Location;
				IconObject->Scale *= 1.0f - (TIMER::GetDeltaSeconds() * 4.0f);
				IconObject->Location += IconVelocity * TIMER::GetDeltaSeconds();

				if (IconObject->Location.y >= Location.y - 18.0f * Scale.x)
				{
					CurrentState = CatState::IconEating;
				}

				break;
			}

			default:
			{
				glDeleteTextures(1, &IconTexture);
				IconTexture = 0;

				delete IconObject;
				IconObject = nullptr;
				return;
			}
		}

		IconObject->Update();
	}
}

float CCat::GetBoundsWidth()
{
	return 20.0f * Scale.x;
}

float CCat::GetBoundsHeight()
{
	return 16.0f * Scale.x;
}

bool CCat::IsInBounds(glm::vec2 location)
{
	float width = GetBoundsWidth();
	float height = GetBoundsHeight();

	glm::vec2 loc = glm::vec2(Location.x - width * 0.5f, Location.y - height);
	glm::vec2 size = glm::vec2(width, height);

	return HELPER::InBounds(location, loc, loc + size);
}

int CCat::GetNumberOfColours()
{
	return NumberOfColours;
}

glm::vec3 CCat::GetColour(int index)
{
	return Colours[index];
}

void CCat::SetColour(int index, glm::vec3 colour)
{
	Colours[index] = colour;
}

HWND CCat::GetCurrentWindow()
{
	return CurrentWindow;
}

void CCat::SetName(std::string name)
{
	Name = name;
	NameDimensions = NameFont->TextBounds(Name, 1.0f);
}

void CCat::SetListIterator(std::list<CCat*>::iterator listIt)
{
	ListIt = listIt;
}

std::string CCat::GetName()
{
	return Name;
}

/// <summary>
/// Check whether the location is overlapping any of the monitors
/// </summary>
/// <param name="location">The location of the cat to check</param>
/// <returns>true if the location is valid</returns>
bool CCat::IsLocationValid(glm::vec3 location)
{
	// All monitors
	std::vector<HMONITOR> monitors = MM.GetMonitors();

	// List of all the intersections with the monitors
	std::vector<RECT> intersections;
	bool isInMonitor = false;

	// Get player boundry
	glm::vec3 oldLoc = Location;
	Location = location;
	RECT bounds = GetBoundsDesk();
	Location = oldLoc;

	// Calculate the players dimensions
	float playerWidth = bounds.right - bounds.left;
	float playerHeight = bounds.bottom - bounds.top;

	// Loop through each monitor
	for (int i = 0; i < (int)monitors.size(); ++i)
	{
		// Retrieve the data of the current monitor
		int monitor = (int)monitors[i];
		MonitorInfo& info = MM.GetMonitorDetail(monitor);

		// Get the intersection rectangle between the monitor and the cat
		RECT tempIntersection;
		IntersectRect(&tempIntersection, &info.BoundryTB, &bounds);

		float width = tempIntersection.right - tempIntersection.left;
		float height = tempIntersection.bottom - tempIntersection.top;

		// Check whether the player needs to move at all
		if (width == playerWidth &&
			height == playerHeight)
		{
			// Return out early as no intersection occurs
			return true;
		}
		else if (!isInMonitor && height != 0 || width != 0)
		{
			isInMonitor = true;
		}
		else
		{
			intersections.push_back(tempIntersection);
		}
	}

	if (!isInMonitor)
	{
		return false;
	}

	// Split the collision into 4 sections (this is because anymore is unlikely to happen in terms of monitors)
	float topLeftW = 0.0f;
	float topLeftH = 0.0f;

	float topRightW = 0.0f;
	float topRightH = 0.0f;

	float botLeftW = 0.0f;
	float botLeftH = 0.0f;

	float botRightW = 0.0f;
	float botRightH = 0.0f;

	// Loop through each intersection
	for (int i = 0; i < intersections.size(); ++i)
	{
		// Retrieve the data of the current intersection
		RECT intersec = intersections[i];
		float width = intersec.right - intersec.left;
		float height = intersec.bottom - intersec.top;

		// Check where the intersection takes place
		bool left = bounds.left == intersec.left;
		bool right = bounds.right == intersec.right;
		bool top = bounds.top == intersec.top;
		bool bottom = bounds.bottom == intersec.bottom;

		// Update the corrosponding segments

		if (left && right)
		{
			if (top)
			{
				topLeftW = width;

				topLeftH = height;
				topRightH = height;
			}
			else
			{
				botLeftW = width;

				botLeftH = height;
				botRightH = height;
			}
		}
		else if (top && bottom)
		{
			if (left)
			{
				topLeftW = width;
				botLeftW = width;

				topLeftH = height;
			}
			else
			{
				topRightW = width;
				botRightW = width;

				topRightH = height;
			}
		}
		else
		{
			if (left)
			{
				if (top)
				{
					topLeftW = width;
					topLeftH = height;
				}
				else if (bottom)
				{
					botLeftW = width;
					botLeftH = height;
				}
			}

			if (right)
			{
				if (top)
				{
					topRightW = width;
					topRightH = height;
				}
				else if (bottom)
				{
					botRightW = width;
					botRightH = height;
				}
			}
		}
	}

	// Retrieve the combined width and height of the intersections
	float combinedWidth = std::min(topLeftW + topRightW, botLeftW + botRightW);
	float combinedHeight = std::min(topLeftH + botLeftH, topRightH + botRightH);

	// Check for a difference between the intersection and the player dimensions
	if (combinedWidth != playerWidth ||
		combinedHeight != playerHeight)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CCat::SetCurrentState(CatState newState)
{
	CurrentState = newState;
}
CatState CCat::GetCurrentState()
{
	return CurrentState;
}

void CCat::QueueAction(Action* newAction)
{
	Actions.push(newAction);
}

CConfigs* CCat::GetConfigs()
{
	return Configs;
}

void CCat::Duplicate()
{
	ClearActions();
	Velocity = HELPER::RandomVecInRadius(1.0f);
	Velocity.y = std::abs(Velocity.y);
	Velocity = HELPER::SetMag(Velocity, 1000.0f);

	CCat* other = ParentManager->AddCat(glm::vec2(Location), Configs);
	other->Velocity = glm::vec2(-Velocity.x, Velocity.y);
}

void CCat::Delete()
{
	ParentManager->DeleteCat(ListIt);
}

bool CCat::TryJumpToWindow()
{
	// Retrieve and shuffle windows so other windows (not just the same one) get a chance
	std::vector<Platform> windows = WM.GetPlatforms();

	std::vector<int> iteration(windows.size());
	std::iota(iteration.begin(), iteration.end(), 0);
	std::shuffle(iteration.begin(), iteration.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

	// Loop through each window
	for (int i : iteration)
	{
		Platform p = windows[i];
		HWND hWnd = p.Hwnd;

		// Make sure the window exists and that it is not the window currently stood upon
		if (CurrentWindow != hWnd)
		{
			// Check if the window is low enough to jump to
			if (p.Height < Location.y &&
				p.Height >= Location.y - JumpHeight)
			{
				// Retrieve the bounds of the window currently stood upon
				RECT sourceBounds;
				if (CurrentWindow != nullptr && WM.GetWindowBoundryLocalCoords(CurrentWindow, sourceBounds))
				{
					// Make sure that the cat has a spot to stand on
					if (sourceBounds.left < p.Right && sourceBounds.right > p.Left)
					{
						// Find the exact location to jump up
						float targetX = std::clamp(Location.x, (float)p.Left, (float)p.Right);

						// Make sure the locations is valid (not intersecting with monitors)
						if (IsLocationValid(glm::vec3(targetX, p.Height - 10, 0.0f)))
						{
							// Walk to and jump onto the window!
							Actions.push(new WalkToWindowAction(this, hWnd));
							JumpingAction::QueueJumpingActions(this, hWnd);
							return true;
						}
					}
				}
				else
				{
					// Find the exact location to jump up
					float targetX = std::clamp(Location.x, (float)p.Left, (float)p.Right);

					// Make sure the locations is valid (not intersecting with monitors)
					if (IsLocationValid(glm::vec3(targetX, p.Height - 10, 0.0f)))
					{
						// Walk to and jump onto the window!
						Actions.push(new WalkToWindowAction(this, hWnd));
						JumpingAction::QueueJumpingActions(this, hWnd);
						return true;
					}
				}
			}
		}
	}

	// No valid windows found
	return false;
}

std::vector<std::pair<std::string, float>> CCat::GetIdleAnimations()
{
	std::vector<std::pair<std::string, float>> idleAnimations;

	// Loop through each animation
	for (std::map<const std::string, SSpriteAnimation>::iterator it = Animations.begin(); it != Animations.end(); it++)
	{
		// Check if the name starts with Idle
		if (it->first.rfind("Idle ", 0) == 0)
		{
			// Calculate the length of the animation and add it to the return list
			float animationLength = (it->second.Frames.size() / it->second.FPS) * (it->second.AnimationType == EAnimationType::PingPong ? 2.0f : 1.0f);
			idleAnimations.push_back(std::make_pair(it->first, animationLength));
		}
	}

	return idleAnimations;
}

RECT CCat::GetBoundsWind()
{
	glm::vec2 loc = glm::vec2(Location);
	float halfWidth = GetBoundsWidth() * 0.5f;

	glm::vec2 topLeft = loc - glm::vec2(halfWidth, GetBoundsHeight());
	glm::vec2 botRight = loc + glm::vec2(halfWidth, 0.0f);

	return RECT{ (int)std::ceil(topLeft.x), (int)std::ceil(topLeft.y), (int)std::ceil(botRight.x), (int)std::ceil(botRight.y) };
}

RECT CCat::GetBoundsDesk()
{
	glm::vec2 loc = MM.WindowToDesktop(glm::vec2(Location));
	float halfWidth = GetBoundsWidth() * 0.5f;

	glm::vec2 topLeft = loc - glm::vec2(halfWidth, GetBoundsHeight());
	glm::vec2 botRight = loc + glm::vec2(halfWidth, 0.0f);

	return RECT{ (int)std::ceil(topLeft.x), (int)std::ceil(topLeft.y), (int)std::ceil(botRight.x), (int)std::ceil(botRight.y) };
}

void CCat::ClearActions()
{
	delete CurrentAction;
	CurrentAction = nullptr;

	while (!Actions.empty())
	{
		delete Actions.front();
		Actions.pop();
	}
}

void CCat::ClearSubsequentActions()
{
	while (!Actions.empty())
	{
		delete Actions.front();
		Actions.pop();
	}
}

void CCat::GenerateAnimations()
{
	CTexture* bouncingTexture = CTexture::GetTexture("CatRolling");
	SSpriteAnimation bouncing = SSpriteAnimation{ bouncingTexture, 1, 1, bouncingTexture->GetDimensions() / glm::ivec2(2, 2), { { 0, 0 } }, EAnimationType::Loop, 0.0f };
	Animations.insert({ "Bouncing", bouncing });

	CTexture* slidingTexture = CTexture::GetTexture("CatSliding");
	SSpriteAnimation slidingL = SSpriteAnimation{ slidingTexture, 1, 1, slidingTexture->GetDimensions() / glm::ivec2(2, 1), { { 0, 0 } }, EAnimationType::Loop, 0.0f };
	SSpriteAnimation slidingR = SSpriteAnimation{ slidingTexture, 1, 1, slidingTexture->GetDimensions() / glm::ivec2(2, 1), { { 0, 0 } }, EAnimationType::Loop, 0.0f, true };
	Animations.insert({ "Sliding Left", slidingL });
	Animations.insert({ "Sliding Right", slidingR });

	CTexture* sleepingTexture = CTexture::GetTexture("SleepingSpritesheet");
	SSpriteAnimation sleeping = SSpriteAnimation{ sleepingTexture, 3, 1, glm::vec2((sleepingTexture->GetWidth() / 3) * 0.5f, sleepingTexture->GetHeight()), { { 0, 0 }, { 1, 0 }, { 2, 0 } }, EAnimationType::PingPong, 1.0f };
	Animations.insert({ "Sleeping", sleeping });

	CTexture* idleTexture = CTexture::GetTexture("IdleSpritesheet");
	glm::vec2 idleOffset = glm::vec2((idleTexture->GetWidth() / 3) * 0.5f, (idleTexture->GetHeight() / 3.0f));
	SSpriteAnimation idle = SSpriteAnimation{ idleTexture, 3, 3, idleOffset, { { 0, 0 } }, EAnimationType::Loop, 0.0f };
	Animations.insert({ "Idle", idle });

	SSpriteAnimation idleBlink = SSpriteAnimation{ idleTexture, 3, 3, idleOffset, { { 0, 0 }, { 1, 0 }, { 0, 0 } }, EAnimationType::Toggle, 2.0f };
	Animations.insert({ "Idle Blink", idleBlink });

	SSpriteAnimation idleTail = SSpriteAnimation{ idleTexture, 3, 3, idleOffset, { { 0, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 0, 0 } }, EAnimationType::Toggle, 4.0f };
	Animations.insert({ "Idle Tail", idleTail });

	SSpriteAnimation idleYawn = SSpriteAnimation{ idleTexture, 3, 3, idleOffset, { { 0, 0 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 1, 2 }, { 0, 2 }, { 0, 0 }, }, EAnimationType::Toggle, 4.0f };
	Animations.insert({ "Idle Yawn", idleYawn });

	CTexture* walkingTexture = CTexture::GetTexture("CatSpriteSheet_v1");
	std::vector<glm::ivec2> walkingFrames = { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 0, 2 }, { 1, 2 } };

	SSpriteAnimation walkingL = { walkingTexture, 3, 3, glm::vec2((walkingTexture->GetWidth() / 4) * 0.5f, (walkingTexture->GetHeight() / 3.0f)), walkingFrames, EAnimationType::Loop, 12.0f, true };
	Animations.insert({ "Walking Left", walkingL });

	SSpriteAnimation walkingR = { walkingTexture, 3, 3, glm::vec2((walkingTexture->GetWidth() / 4) * 0.5f, (walkingTexture->GetHeight() / 3.0f)), walkingFrames, EAnimationType::Loop, 12.0f };
	Animations.insert({ "Walking Right", walkingR });

	CTexture* jumpStartTexture = CTexture::GetTexture("JumpStartSpritesheet");
	std::vector<glm::ivec2> jumpStartFrames = { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } };
	SSpriteAnimation jumpingStart = SSpriteAnimation{ jumpStartTexture, 3, 2, jumpStartTexture->GetDimensions() / glm::ivec2(3, 2) / glm::ivec2(2, 1), jumpStartFrames, EAnimationType::Toggle, 40.0f };
	Animations.insert({ "Jumping Start", jumpingStart });

	CTexture* jumpMiddleTexture = CTexture::GetTexture("JumpMiddleSpritesheet");
	std::vector<glm::ivec2> jumpMiddleFrames = { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 0, 2 }, { 1, 2 }, { 2, 2 } };
	SSpriteAnimation jumpingMiddle = SSpriteAnimation{ jumpMiddleTexture, 3, 3, jumpMiddleTexture->GetDimensions() / glm::ivec2(3, 3) / glm::ivec2(2, 2), jumpMiddleFrames, EAnimationType::Loop, 10.0f };
	Animations.insert({ "Jumping Middle", jumpingMiddle });

	CTexture* jumpEndTexture = CTexture::GetTexture("JumpEndSpritesheet");
	std::vector<glm::ivec2> jumpEndFrames = { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 } };
	SSpriteAnimation jumpingEnd = SSpriteAnimation{ jumpEndTexture, 3, 2, glm::vec2(jumpEndTexture->GetDimensions()) / glm::vec2(3, 2) * glm::vec2(0.5, 0.65), jumpEndFrames, EAnimationType::Toggle, 30.0f };
	Animations.insert({ "Jumping End", jumpingEnd });

	CTexture* carriedTexture = CTexture::GetTexture("CarriedSpritesheet");
	SSpriteAnimation carried = SSpriteAnimation{ carriedTexture, 3, 2, glm::vec2((carriedTexture->GetWidth() / 3.0f) * 0.5f, (carriedTexture->GetHeight() / 2.0f) * 0.5f), EAnimationType::Loop, 10.0f };
	Animations.insert({ "Carried", carried });

	CTexture* eatingTexture = CTexture::GetTexture("CatEatingSpritesheet");
	SSpriteAnimation eatingIcon = SSpriteAnimation{ eatingTexture, 3, 1, glm::vec2(eatingTexture->GetDimensions()) / glm::vec2(3, 1) * glm::vec2(0.5f, 1.0f), { { 0, 0 }, { 1, 0 },{ 1, 0 }, { 2, 0 } }, EAnimationType::Toggle, 7.0f };
	Animations.insert({ "Eating Icon", eatingIcon });

	//CTexture* fallingTexture = CTexture::GetTexture("CatFalling");
	//SSpriteAnimation falling = SSpriteAnimation{ fallingTexture, 1, 2, glm::vec2(fallingTexture->GetDimensions()) / glm::vec2(1, 2) * glm::vec2(0.5f, 1.0f), { { 0, 0 }, { 0, 1 } }, EAnimationType::Loop, 7.0f };
	//Animations.insert({ "Falling", falling });
	//
	//CTexture* standingTexture = CTexture::GetTexture("CatStanding");
	//SSpriteAnimation standing = SSpriteAnimation{ standingTexture, 1, 1, glm::vec2(standingTexture->GetDimensions()) * glm::vec2(0.5f, 1.0f), { { 0, 0 } }, EAnimationType::Loop, 0.0f };
	//Animations.insert({ "Standing", standing });
}

void CCat::UpdateModelMatrix()
{
	CSprite::UpdateModelMatrix();
	ModelMatrix = glm::translate(glm::mat4(), Offset) * ModelMatrix;
}

void CCat::OnAnimationEnd()
{
	if (CurrentAction != nullptr) { CurrentAction->OnAnimationEnd(); }
}


void CCat::UpdateColours()
{
	for (int i = 1; i <= NumberOfColours; ++i)
	{
		glm::vec3 colour = Colours[i];

		if (colour.r < 0 || colour.g < 0 || colour.b < 0)
		{
			// Increase hue of colour
			colour = glm::abs(colour) * 255.0f;
			glm::vec3 hsv = HELPER::RGBtoHSV(colour.r, colour.g, colour.b);
			hsv.x = std::fmodf(hsv.x + TIMER::GetDeltaSeconds() * 360.0f * 0.5f, 360.0f);
			Colours[i] = HELPER::HSVtoRGB(hsv.x, hsv.y, hsv.z) / -255.0f;
		}
	}
}

GLuint CCat::HiconToTexture(HICON hIcon)
{
	GLuint textureID = 0;

	// Get the data of the icon
	ICONINFO iconData;
	if (::GetIconInfo(hIcon, &iconData) == FALSE) return NULL;

	// Create a new DC to copy to
	HDC mainDC = ::GetDC(NULL);
	HDC memDC = ::CreateCompatibleDC(mainDC);

	if (mainDC == NULL || memDC == NULL) return NULL;

	BITMAP bmp;
	if (::GetObject(iconData.hbmColor, sizeof(BITMAP), &bmp))
	{
		// Dimensions
		DWORD  width = iconData.xHotspot * 2;
		DWORD  height = iconData.yHotspot * 2;

		// Generate the info of the bitmap
		BITMAPINFO mybmi;
		mybmi.bmiHeader.biSize = sizeof(mybmi);
		mybmi.bmiHeader.biWidth = width;
		mybmi.bmiHeader.biHeight = height;
		mybmi.bmiHeader.biPlanes = 1;
		mybmi.bmiHeader.biBitCount = 32;
		mybmi.bmiHeader.biCompression = BI_RGB;
		mybmi.bmiHeader.biSizeImage = ((width * (mybmi.bmiHeader.biBitCount / 8) + 3) & -4) * height;

		// The bits of the texture
		unsigned char* bits;

		HBITMAP newbitmap = CreateDIBSection(memDC, &mybmi, DIB_RGB_COLORS, (VOID**)&bits, 0, 0);
		if (!newbitmap) { return NULL; }

		int result = GetDIBits(memDC, iconData.hbmColor, 0, height, bits, (BITMAPINFO*)&mybmi, DIB_RGB_COLORS);
		if (!result) { return NULL; }

		// Generate a new texture
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Pixel alignment
		glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
		glPixelStorei(GL_UNPACK_LSB_FIRST, GL_TRUE);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);

		// Specify the min mag filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	::DeleteObject(iconData.hbmColor);
	::DeleteObject(iconData.hbmMask);
	::DeleteDC(memDC);
	::ReleaseDC(NULL, mainDC);

	return textureID;
}

void CCat::SpawnHeart()
{
	if (IsSpawningHearts)
	{
		if (GINPUT::GetDeltaMouseLocation() != glm::vec2() && IsInBounds(GINPUT::GetMouseLocation()))
		{
			//Parent->ParticlesLayer->SpawnParticle(glm::vec2(Location), "Hearts");
			Parent->ParticlesLayer->SpawnParticle(GINPUT::GetMouseLocation(), "Hearts");
		}

		SetAlarm(SpawningHeartInterval, std::bind(&CCat::SpawnHeart, this));
	}
}