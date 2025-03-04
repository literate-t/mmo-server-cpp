#pragma once

/*-----------------
	RefCountable
-----------------*/
class RefCountable
{
public:
	RefCountable() : _ref_count(0) {}
	virtual ~RefCountable() = default;

	int32 GetRefCount() { return _ref_count; }
	int32 AddRef() { return ++_ref_count; }
	int32 ReleaseRef()
	{
		int32 ref_count = --_ref_count;
		if (0 == ref_count)
			delete this;

		return ref_count;
	}

protected:
	//int32 _ref_count;
	atomic<int32> _ref_count;
};

/*---------------
	SharedPtr
---------------*/
template<typename T>
class TSharedPtr
{
	// 생성자
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) 
	{ 
		if (nullptr == ptr)
			return;
		
		Set(ptr); 
		printf("TSharedPtr(T* ptr) name: %s\n", typeid(this).name());
	}

	// copy
	TSharedPtr(const TSharedPtr& rhs) 
	{ 
		Set(rhs._ptr); 
		printf("TSharedPtr(const TSharedPtr& rhs) name: %s\n", typeid(this).name());
	}
	// move
	TSharedPtr(const TSharedPtr&& rhs) 
	{ 
		printf("TSharedPtr(const TSharedPtr&& rhs) name: %s\n", typeid(this).name());
		_ptr = rhs._ptr; rhs._ptr = nullptr; 
	}
	// 상속 관계 복사
	// T와 U가 상속 관계인 경우를 말한다
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() 
	{
		Release(); 
	}

	// 연산자
public:
	// copy operator
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		// 자기 대입은 무의미
		if (this == &rhs)
			return *this;

		if (_ptr != rhs._ptr)
			Release();

		Set(rhs._ptr);

		return *this;
	}

	// move operator
	TSharedPtr& operator=(TSharedPtr&& rhs) noexcept
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;

		return *this;
	}

	bool		operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool		operator==(T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool		operator!=(T* ptr) const { return _ptr != ptr; }
	T*			operator*() { return _ptr; }
	const T*	operator*() const { return _ptr; }
				operator T* () const { return _ptr; }
	T*			operator->() { return _ptr; }
	const T*	operator->() const { return _ptr; }

	bool IsNull() { return nullptr == _ptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (nullptr == ptr)
			return;

		ptr->AddRef();
	}

	inline void Init(T* ptr)
	{
		_ptr = ptr;
	}

	inline void Release()
	{
		if (nullptr != _ptr)
		{
			if (0 == _ptr->ReleaseRef())
			{
				printf("Release() name: %s, Reference count: %d\n", typeid(this).name(), 0);
				_ptr = nullptr;
			}
			else
				printf("Release() name: %s, Reference count: %d\n", typeid(this).name(), _ptr->GetRefCount());
		}
	}

private:
	T* _ptr = nullptr;
};