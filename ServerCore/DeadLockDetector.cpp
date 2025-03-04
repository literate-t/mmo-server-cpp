#include "pch.h"

void DeadLockDetector::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 아이디를 찾거나, 없으면 발급
	int32 current_lock_id = 0;

	auto find_it = _name_to_id.find(name);	
	if (_name_to_id.end() == find_it)
	{
		current_lock_id = static_cast<int32>(_name_to_id.size());
		_name_to_id[name] = current_lock_id;
		_id_to_name[current_lock_id] = name;
	}
	else
	{
		current_lock_id = find_it->second;
	}

	// 잡고 있는 락이 있다면
	if (false == tls_lock_stack.empty())
	{
		// 락을 걸 때마다 탐지할 건데,
		// 동일 스레드에서 중첩해서 거는 락이면 사이클 검사 할 필요 없다
		const int32 last_lock_id = tls_lock_stack.top();
		if (current_lock_id != last_lock_id)
		{
			// key에 해당하는 value를 가지고 있지 않으면 생성해서 반환한다(map, unordered_map 모두 해당)
			set<int32>& history = _lock_history[last_lock_id];

			// current_lock_id가 새로운 것이면 히스토리에 넣고 전체 사이클을 탐지하며
			// 새로운 것이 아니면 탐지하지 않는다(이미 했으니까)
			if (history.find(current_lock_id) == history.end())
			{
				history.insert(current_lock_id);
				CheckCycle();
			}
		}
	}

	// 이게 살짝 이상한 느낌이 있었는데
	// 프로젝트에서는 _lock_stack을 tls에서 관리한다고
	tls_lock_stack.push(current_lock_id);
}

void DeadLockDetector::PopLock(const char* name)
{
	// 잡은 lock이 없는데 pop을 하는 건 잘못된 동작
	LockGuard guard(_lock);
	if (tls_lock_stack.empty())
		CRASH("Multiple unlock");

	// 1,2,3,4로 lock을 잡았으면 4,3,2,1로 unlock을 해야 하는데
	// 이 순서가 틀리면 잘못된 동작
	int32 lock_id = _name_to_id[name];
	if (tls_lock_stack.top() != lock_id)
		CRASH("Invalid unlock");

	tls_lock_stack.pop();
}

void DeadLockDetector::CheckCycle()
{
	const int32 lock_count = static_cast<int32>(_name_to_id.size());
	_discovered_order = vector<int32>(lock_count, -1);
	_discovered_count = 0;
	_finished = vector<bool>(lock_count, false);
	_parent = vector<int32>(lock_count, -1);

	for (int32 lock_id = 0; lock_id < lock_count; ++lock_id)
		Dfs(lock_id);
}

void DeadLockDetector::Dfs(int32 here)
{
	// 방문한 정점은 탐색하지 않는다
	if (-1 != _discovered_order[here])
		return;

	_discovered_order[here] = _discovered_count++;

	set<int32>& connected = _lock_history[here];
	for (int32 there : connected)
	{
		// tree edge
		if (-1 == _discovered_order[there])
		{
			_parent[there] = here;
			Dfs(there);
		}
		// forward edge(here가 there보다 방문 순서가 빠르면)
		else if (_discovered_order[here] < _discovered_order[there])
			continue;
		// backward edge. cycle! (dfs 순회가 끝나지 않았는데 there가 here보다 방문 순서가 빠르면)
		else if (false == _finished[there])
		{
			printf("%s -> %s\n", _id_to_name[here], _id_to_name[there]);
			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _id_to_name[_parent[now]], _id_to_name[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("Lock Cycle Detection!");
		}
	}

	_finished[here] = true;
}