#pragma once

struct RewardData;
class DBSerializer : public JobQueue
{
public:	
	~DBSerializer() = default;
	DBSerializer(const DBSerializer&) = delete;
	DBSerializer& operator=(const DBSerializer&) = delete;

	static shared_ptr<DBSerializer> Instance()
	{
		static shared_ptr<DBSerializer> instance(new DBSerializer());
		return instance;
	}

	static void SavePlayerStatusAll(SharedPlayer player);
	static void SavePlayerReward(SharedPlayer player, SharedRoom room, RewardData reward);

private:
	DBSerializer() = default;
};
