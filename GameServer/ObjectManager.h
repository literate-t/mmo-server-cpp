#pragma once
#include "Protocol.pb.h"
using namespace Protocol;
#include "GameObject.h"

class ObjectManager
{
public:
	template<typename T>
	shared_ptr<T> Add()
	{
		static_assert(is_base_of_v<GameObject, T>, "T type must be derived from GameObject");
		static_assert(is_default_constructible_v<T>, "T type must be default constructible");

		shared_ptr<T> object = make_shared<T>();

		{
			WRITE_LOCK;
			object->SetObjectId(GenerateId(object->GetType()));
		}
		
		return object;
	}

	int32 GenerateId(GameObjectType type);
	static GameObjectType GetObjectTypeById(int32 object_id);

private:
	// [UNUSED(1)|TYPE(7)][      ID(24)      ]
	int32 _counter = 1;

	USE_LOCK;
};

