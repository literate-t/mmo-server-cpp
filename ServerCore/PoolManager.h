// ------- PoolManager ------- //
class PoolManager
{
	enum
	{
		// ~1024까지는 32단위, ~2048까지는 128단위, ~4096까지 256단위로 증가
		// 단위가 작을수록 촘촘하게
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	PoolManager();
	~PoolManager();

	inline static PoolManager& Instance()
	{
		static PoolManager instance;
		return instance;
	}

	void* Allocate(int32 size);
	void	Release(void* ptr);

private:
	vector<class MemoryPool*> _pools;

	// O(1)을 위한 pool table
	// 1 ~ 4096
	class MemoryPool* _pool_table[MAX_ALLOC_SIZE + 1] = {};
};