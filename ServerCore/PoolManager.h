// ------- PoolManager ------- //
class PoolManager
{
	enum
	{
		// ~1024������ 16����, ~2048������ 64����, ~4096���� 128������ ����
		// ������ �������� �����ϰ�
		POOL_COUNT = (1024 / 32) + (1024 / 64) + (2048 / 128),
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

	// O(1)�� ���� pool table
	// 1 ~ 4096
	class MemoryPool* _pool_table[MAX_ALLOC_SIZE + 1] = {};
};