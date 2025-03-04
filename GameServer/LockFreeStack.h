#pragma once

/*--------------------
  1차 시도(싱글 스레드)
--------------------*/
//struct ListEntry
//{
//	ListEntry* next = nullptr;
//};

//// 메모리 풀링을 진행할 객체에 대한 데이터를 직접 가진다
//struct Data
//{
//	ListEntry _entry;
//
//	int32 _hp;
//	int32 _mp;
//};

// Header의 next가 ListEntry의 첫 번째 데이터를 가리킨다
// [Header->next] -> [][][][]
//struct ListHeader
//{
//	ListEntry* next = nullptr;
//};

//void InitializeHeader(ListHeader* header);
//void PushToEntryList(ListHeader* header, ListEntry* entry);
//ListEntry* PopFromEntryList(ListHeader* header);

///*--------------------
//  2차 시도(멀티 스레드)
//--------------------*/
//void InitializeHeader(ListHeader* header);
//void PushToEntryList(ListHeader* header, ListEntry* entry);
//ListEntry* PopFromEntryList(ListHeader* header);

/*--------------------
  3차 시도(ABA 해결)
--------------------*/
// 메모리를 16바이트 정렬하도록
__declspec(align(16))
struct ListEntry
{
	ListEntry* next = nullptr;
};

// 메모리를 16바이트 정렬하도록
__declspec(align(16))
struct ListHeader
{
	ListHeader()
	{
		alignment = 0;
		region = 0;
	}

	union
	{
		struct
		{
			uint64 alignment;
			uint64 region;
		};

		struct
		{
			uint64 depth	:	16;
			uint64 sequence :	48;
			uint64 reserve	:	4;
			uint64 next		:	60;
		} HeaderX64;

	};
};

void InitializeHeader(ListHeader* header);
void PushToEntryList(ListHeader* header, ListEntry* entry);
ListEntry* PopFromEntryList(ListHeader* header);