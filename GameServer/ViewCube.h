#pragma once
class ViewCube
{
public:
	ViewCube(SharedPlayer player);

	void Update();
	const xhash_set<SharedObject> GetObjects();

public:
	bool IsReset;
	static constexpr int32 kViewRange = 10;
	SharedJob JobReserved;

private:
	const xvector<SharedObject> Except(const xhash_set<SharedObject>& lhs, const xhash_set<SharedObject>& rhs);

private:
	// caution: cycle
	SharedPlayer _owner;
	SharedJob _job;
	xhash_set<SharedObject> _prev_objects;

	USE_LOCK;
};

