#pragma once

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : _callback(move(callback)) {}

	template<typename T, typename Return, typename...Args1, typename...Args2>
	Job(shared_ptr<T> obj, Return(T::*member_func)(Args1...), Args2&&...args)
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

	void Cancel();

private:
	CallbackType _callback;
};

