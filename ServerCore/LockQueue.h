#pragma once

template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		WRITE_LOCK;
		_item.push(item);
	}

	T Pop()
	{
		T result;
		{
			WRITE_LOCK;
			if (_item.empty())
				return T();

			result = _item.front();
			_item.pop();
		}

		return result;
	}

	void PopAll(OUT xvector<T>& items)
	{
		items.reserve(_item.size());
		WRITE_LOCK;
		while (T item = Pop())
		{
			items.push_back(item);
		}

	}

	void Clear()
	{
		WRITE_LOCK;
		_item = xqueue<T>();
	}

private:
	USE_LOCK;
	xqueue<T> _item;
};
