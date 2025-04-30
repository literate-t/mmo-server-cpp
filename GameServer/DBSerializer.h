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

	static void SavePlayerStatusNoti(SharedPlayer player);
	static void SavePlayerReward(SharedPlayer player, SharedRoom room, RewardData reward);
	static void EquipItemNoti(SharedPlayer player, SharedItem item);
	static void DropItemNoti(SharedPlayer player, SharedItem item);

private:
	DBSerializer() = default;
};
