#pragma once
class ViewCube
{
public:
	ViewCube(SharedPlayer player);

	void Update();
	const unordered_set<SharedObject>& GetObjects();

public:
	bool IsReset;

private:
	// caution: cycle
	SharedPlayer _owner_player;
	unordered_set<SharedObject> _prev_objects;
	SharedJob _job;
	unordered_set<SharedObject> _objects;
};

