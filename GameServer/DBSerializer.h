#pragma once

class DBSerializer : public JobQueue
{
public:	
	~DBSerializer() = default;
	DBSerializer(const DBSerializer&) = delete;
	DBSerializer& operator=(const DBSerializer&) = delete;

	static DBSerializer& Instance()
	{
		static DBSerializer instance;
		return instance;
	}

	static void SavePlayerStatusAll(SharedPlayer player, SharedRoom room);

private:
	DBSerializer() = default;
};
