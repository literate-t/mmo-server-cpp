#pragma once
class ViewCube
{
public:
	ViewCube(SharedPlayer player);

	void Update();
	const xhash_set<SharedObject>& GetObjects();

public:
	bool IsReset;

private:
	// caution: cycle
	SharedPlayer _owner_player;
	xhash_set<SharedObject> _prev_objects;
	SharedJob _job;
	xhash_set<SharedObject> _objects;
};

