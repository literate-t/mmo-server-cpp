#include "pch.h"
#include "LockFreeStack.h"

/*--------------------
  1차 시도(싱글 스레드)
--------------------*/
//void InitializeHeader(ListHeader* header)
//{
//	header->next = nullptr;
//}
//
//void PushToEntryList(ListHeader* header, ListEntry* entry)
//{
//	entry->_next = header->next;
//	header->next = entry;
//}
//
//ListEntry* PopFromEntryList(ListHeader* header)
//{
//	ListEntry* first = header->next;
//	if (nullptr != first)
//	{
//		header->next = first->next;
//	}
//
//	return first;
//}

/*--------------------
  2차 시도(멀티 스레드)
--------------------*/
//void InitializeHeader(ListHeader* header)
//{
//	header->next = nullptr;
//}
//
//void PushToEntryList(ListHeader* header, ListEntry* entry)
//{
//	do
//	{
//		entry->next = header->next;
//	} while ((int64)entry->next != InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next));
//}
//
//ListEntry* PopFromEntryList(ListHeader* header)
//{
//	ListEntry* first = nullptr;
//	do
//	{
//		first = header->next;
//
//	} while (first && (int64)first != InterlockedCompareExchange64((int64*)&header->next, (int64)first->next, (int64)first));
//
//	return first;
//}

/*--------------------
  3차 시도(ABA 해결)
--------------------*/
void InitializeHeader(ListHeader* header)
{
	header->alignment = 0;
	header->region = 0;
}

void PushToEntryList(ListHeader* header, ListEntry* entry)
{
	ListHeader original = {};
	ListHeader new_header = {};

	new_header.HeaderX64.next = (uint64)entry >> 4;

	do
	{
		original = *header;

		// 60비트로 표현된 주소값을 다시 64비트로 만든다
		entry->next = (ListEntry*)(original.HeaderX64.next << 4);
		new_header.HeaderX64.depth = original.HeaderX64.depth + 1;
		new_header.HeaderX64.sequence = original.HeaderX64.sequence + 1;

	} while (1 != ::InterlockedCompareExchange128((int64*)header, new_header.region, new_header.alignment, (int64*)&original));
}

ListEntry* PopFromEntryList(ListHeader* header)
{	
	ListHeader original = {};
	ListHeader new_header = {};
	ListEntry* first_entry = nullptr;

	do
	{
		original = *header;

		first_entry = (ListEntry*)(original.HeaderX64.next << 4);
		if (nullptr == first_entry) return nullptr;

		// first_entry->next에서 User-After-Free 문제가 발생할 수 있는데..
		new_header.HeaderX64.next = (uint64)first_entry->next >> 4;
		new_header.HeaderX64.depth = original.HeaderX64.depth - 1;
		new_header.HeaderX64.sequence = original.HeaderX64.sequence + 1;

	} while (1 != ::InterlockedCompareExchange128((int64*)header, new_header.region, new_header.alignment, (int64*)&original));

	return first_entry;
}