#pragma once

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : _callback(move(callback)) {}

	template<typename T, typename Return, typename...Args>
	Job(shared_ptr<T> obj, Return(T::*member_func)(Args...), Args&&...args)
	{
		_callback = [obj, member_func, args...]()
			{
				(obj.get()->*member_func)(args...);
			};
	}

	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};

