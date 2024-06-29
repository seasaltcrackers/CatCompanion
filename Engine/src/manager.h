#pragma once
#include <vector>
#include <string>

class CRoom;
class CCamera;
class ILayer;

class CManager
{
public:
	unsigned int ScreenWidth = 0;
	unsigned int ScreenHeight = 0;

	std::vector<CRoom*> Rooms;
	std::vector<ILayer*> GlobalLayers;

	virtual void Initialise(); // Initialise all values
	virtual void Update(); // Update all values
	virtual void Render(); // Render objects
	virtual void CleanUp(); // Clean up memory

	virtual void SetRoom(int index);
	virtual void NextRoom();

protected:
	int CurrentRoomIndex = 0;
};
