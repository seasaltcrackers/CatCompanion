#pragma once
#include <string>

class CObject;

namespace TIMER
{
	void Initialise(); //Initialise or restart the start time
	void Update();

	float GetTotalSeconds();
	float GetTotalMilliseconds();

	float GetDeltaSeconds();
	float GetDeltaMilliseconds();

	void AssignTotalSeconds(CObject* anObj, std::string uniformName);
	void AssignTotalMilliseconds(CObject* anObj, std::string uniformName);
	void AssignDeltaSeconds(CObject* anObj, std::string uniformName);
	void AssignDeltaMilliseconds(CObject* anObj, std::string uniformName);
}