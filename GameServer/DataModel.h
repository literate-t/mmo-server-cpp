#pragma once
#include "pch.h"

namespace DataModel
{
	struct ItemDB
	{
		int32 ItemDbId;
		int32 DataSheetId;
		int32 Count;
		int32 Slot;
		bool Equipped;
		int64 OwnerId;
	};
}
