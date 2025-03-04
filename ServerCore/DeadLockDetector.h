#pragma once

/*---------------------
	DeadLockDetector
---------------------*/
class DeadLockDetector
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32>	_name_to_id;
	unordered_map<int32, const char*>	_id_to_name;	
	// (0, {1,2})
	map<int32, set<int32>>				_lock_history;

	Mutex _lock;

private:	
	vector<int32>	_discovered_order;		// ����� �湮 ������ ����	
	int32			_discovered_count = 0;	// ������ ī�����ϴ� ����	
	vector<bool>	_finished;				// Dfs(i)�� ���� ����
	vector<int32>	_parent;
};

