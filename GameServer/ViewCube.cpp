#include "pch.h"
#include "ViewCube.h"

ViewCube::ViewCube(SharedPlayer player)
	:_owner_player(player), IsReset(false)
{	
}

void ViewCube::Update()
{
}

const unordered_set<SharedObject>& ViewCube::GetObjects()
{

	return _objects;
}
