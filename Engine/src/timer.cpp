#include <chrono>

#include "timer.h"
#include "object.h"

// Variables to contain time values so they don't have to be calculated every time you want it
static std::chrono::steady_clock::time_point StartTime;
static std::chrono::steady_clock::time_point OldTime;

static float TotalSeconds = 0.0f;
static float TotalMilliseconds = 0.0f;

static float DeltaSeconds = 0.0f;
static float DeltaMilliseconds = 0.0f;

/*
	Sets the start time of the application, this can be called again to reset the total times
*/
void TIMER::Initialise()
{
	StartTime = std::chrono::high_resolution_clock::now();
	OldTime = StartTime;
}

/*
	Updates the delta and total times
*/
void TIMER::Update()
{
	std::chrono::steady_clock::time_point newTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float, std::milli> tempDeltaMilli = newTime - OldTime;
	std::chrono::duration<float> tempDeltaSec = newTime - OldTime;

	std::chrono::duration<float, std::milli> tempTotalMilli = newTime - StartTime;
	std::chrono::duration<float> tempTotalSec = newTime - StartTime;

	OldTime = newTime;

	DeltaSeconds = tempDeltaSec.count();
	DeltaMilliseconds = tempDeltaMilli.count();

	TotalSeconds = tempTotalSec.count();
	TotalMilliseconds = tempTotalMilli.count();

}

/*
	Return the total seconds

	@return Float of seconds passed since the start time
*/
float TIMER::GetTotalSeconds()
{
	return TotalSeconds;
}
/*
	Return the total milliseconds

	@return Float of milliseconds passed since the start time
*/
float TIMER::GetTotalMilliseconds()
{
	return TotalMilliseconds;
}

/*
	Return the delta seconds

	@return Float of seconds passed since the last frame
*/
float TIMER::GetDeltaSeconds()
{
	return DeltaSeconds;
}

/*
	Return the delta milliseconds

	@return Float of milliseconds passed since the last frame
*/
float TIMER::GetDeltaMilliseconds()
{
	return DeltaMilliseconds;
}

void TIMER::AssignTotalSeconds(CObject* anObj, std::string uniformName)
{
	anObj->UpdateUniformFloatR(uniformName, TotalSeconds);
}

void TIMER::AssignTotalMilliseconds(CObject* anObj, std::string uniformName)
{
	anObj->UpdateUniformFloatR(uniformName, TotalMilliseconds);
}

void TIMER::AssignDeltaSeconds(CObject* anObj, std::string uniformName)
{
	anObj->UpdateUniformFloatR(uniformName, DeltaSeconds);
}

void TIMER::AssignDeltaMilliseconds(CObject* anObj, std::string uniformName)
{
	anObj->UpdateUniformFloatR(uniformName, DeltaMilliseconds);
}
