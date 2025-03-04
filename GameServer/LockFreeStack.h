#pragma once

/*--------------------
  1�� �õ�(�̱� ������)
--------------------*/
//struct ListEntry
//{
//	ListEntry* next = nullptr;
//};

//// �޸� Ǯ���� ������ ��ü�� ���� �����͸� ���� ������
//struct Data
//{
//	ListEntry _entry;
//
//	int32 _hp;
//	int32 _mp;
//};

// Header�� next�� ListEntry�� ù ��° �����͸� ����Ų��
// [Header->next] -> [][][][]
//struct ListHeader
//{
//	ListEntry* next = nullptr;
//};

//void InitializeHeader(ListHeader* header);
//void PushToEntryList(ListHeader* header, ListEntry* entry);
//ListEntry* PopFromEntryList(ListHeader* header);

///*--------------------
//  2�� �õ�(��Ƽ ������)
//--------------------*/
//void InitializeHeader(ListHeader* header);
//void PushToEntryList(ListHeader* header, ListEntry* entry);
//ListEntry* PopFromEntryList(ListHeader* header);

/*--------------------
  3�� �õ�(ABA �ذ�)
--------------------*/
// �޸𸮸� 16����Ʈ �����ϵ���
__declspec(align(16))
struct ListEntry
{
	ListEntry* next = nullptr;
};

// �޸𸮸� 16����Ʈ �����ϵ���
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