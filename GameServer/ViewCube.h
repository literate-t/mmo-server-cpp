#pragma once
class ViewCube
{
public:
	ViewCube(SharedPlayer player);

	void Update();
public:
	bool IsReset;
private:
	// caution: cycle
	SharedPlayer _owner_player;
};

