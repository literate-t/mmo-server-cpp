#include "pch.h"

void DeadLockDetector::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// ���̵� ã�ų�, ������ �߱�
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

	// ��� �ִ� ���� �ִٸ�
	if (false == tls_lock_stack.empty())
	{
		// ���� �� ������ Ž���� �ǵ�,
		// ���� �����忡�� ��ø�ؼ� �Ŵ� ���̸� ����Ŭ �˻� �� �ʿ� ����
		const int32 last_lock_id = tls_lock_stack.top();
		if (current_lock_id != last_lock_id)
		{
			// key�� �ش��ϴ� value�� ������ ���� ������ �����ؼ� ��ȯ�Ѵ�(map, unordered_map ��� �ش�)
			set<int32>& history = _lock_history[last_lock_id];

			// current_lock_id�� ���ο� ���̸� �����丮�� �ְ� ��ü ����Ŭ�� Ž���ϸ�
			// ���ο� ���� �ƴϸ� Ž������ �ʴ´�(�̹� �����ϱ�)
			if (history.find(current_lock_id) == history.end())
			{
				history.insert(current_lock_id);
				CheckCycle();
			}
		}
	}

	// �̰� ��¦ �̻��� ������ �־��µ�
	// ������Ʈ������ _lock_stack�� tls���� �����Ѵٰ�
	tls_lock_stack.push(current_lock_id);
}

void DeadLockDetector::PopLock(const char* name)
{
	// ���� lock�� ���µ� pop�� �ϴ� �� �߸��� ����
	LockGuard guard(_lock);
	if (tls_lock_stack.empty())
		CRASH("Multiple unlock");

	// 1,2,3,4�� lock�� ������� 4,3,2,1�� unlock�� �ؾ� �ϴµ�
	// �� ������ Ʋ���� �߸��� ����
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
	// �湮�� ������ Ž������ �ʴ´�
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
		// forward edge(here�� there���� �湮 ������ ������)
		else if (_discovered_order[here] < _discovered_order[there])
			continue;
		// backward edge. cycle! (dfs ��ȸ�� ������ �ʾҴµ� there�� here���� �湮 ������ ������)
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