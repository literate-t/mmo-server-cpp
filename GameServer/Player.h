#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	uint64 PlayerDbId = 0;
	string name;
	// caution: cycle
	SharedGameSession OwnerSession;

};