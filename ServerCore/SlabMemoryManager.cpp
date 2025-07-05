#include "pch.h"
#include "SlabMemoryManager.h"
#include "SlabPool.h"

const int32 kMallocAlignment = 64;

// ------- SizeConverter ------- //
SizeConverter::SizeConverter()
    :_size_converter{}
{
    generate(arr_size.begin(), arr_size.end(), []() { static int32 seed; return seed += 32; });

    int32 size = 1;
    int32 index = 0;
    for (int32 i = kBlockStride; i <= kMaxBlockSize; i+= kBlockStride)
    {
        while (size <= i)
            _size_converter[size++] = index;
        ++index;
    }
}

// ------- ThreadLocalSlab ------- //
ThreadLocalSlab::ThreadLocalSlab(int32 alloc_size)
    : _block_size(alloc_size),
    _block_count(CalcBlockCountOfSlab(alloc_size))
{    
    _slab_infos.reserve(kMaxSlabInfoCount);

    Fetch();
}

ThreadLocalSlab::~ThreadLocalSlab()
{
}

// BlockHeader를 포함한 크기를 할당하게 됨
void* ThreadLocalSlab::Acquire()
{
    if (_free_block_payloads.empty())
        Fetch();

    void* block = _free_block_payloads.back();
    _free_block_payloads.pop_back();

    return block;
}

void ThreadLocalSlab::Release()
{
    for (int32 i = 0; i < _slab_infos.size();/*none*/)
    {
        if (_slab_infos[i]->block_count == _slab_infos[i]->freed_count.load(memory_order_acquire))
        {
            Flush(_slab_infos[i]);

            _slab_infos[i] = _slab_infos.back();
            _slab_infos.pop_back();
        }
        else
        {
            ++i;
        }
    }
}

void ThreadLocalSlab::Flush(SlabInfo* slab_info)
{    
    SlabMemoryManager::Instance().DrainSlab(slab_info->slab_base);
}

void ThreadLocalSlab::Fetch()
{
    SlabInfo* slab_info = SlabMemoryManager::Instance().RefillSlab(_block_size);
    
    _slab_infos.push_back(slab_info);

    int8* slab_payload = reinterpret_cast<int8*>(SlabMetadata::DetachPayload(slab_info));

    for (int32 i = 0; i < _block_count; ++i)
    {
        void* memory = reinterpret_cast<void*>(slab_payload + (i * _block_size));
        void* block = SlabMetadata::InitHeader(memory, slab_info);


        _free_block_payloads.push_back(block);
    }
}

// ------- SlabMemoryManager ------- //
SlabMemoryManager::SlabMemoryManager()
{
    int32 size = 1;
    for (int32 block = 32; block <= kMaxBlockSize; block += kBlockStride)
    {
        SlabPool* pool = new SlabPool(block);
        while (size <= block)
            _slab_pools[size++] = pool;
    }
}

SlabMemoryManager::~SlabMemoryManager()
{
    for (int32 block = 32; block <= kMaxBlockSize; block += kBlockStride)
        delete _slab_pools[block];
}

void* SlabMemoryManager::Acquire(int32 size)    
{
    int32 alloc_size = size + sizeof(SlabMetadata);
    if (alloc_size > kMaxBlockSize)
        return AcquireBlock(alloc_size);
    
    int32 index = SizeConverter::Instance().ToIndex(alloc_size);
    return tls_slab[index]->Acquire();
}

void SlabMemoryManager::Release(void* block_payload)
{
    SlabMetadata* header = SlabMetadata::DetachHeader(block_payload);
    auto& slab_info = header->slab_info;
    int32 alloc_size = slab_info->alloc_size;
    ASSERT_CRASH(alloc_size > 0);

    if (alloc_size > kMaxBlockSize)
        ReleaseBlock(header);
    else
    {        
        slab_info->freed_count.fetch_add(1, memory_order_acq_rel);
        int32 index = SizeConverter::Instance().ToIndex(slab_info->alloc_size);
        tls_slab[index]->Release();
    }
}

SlabInfo* SlabMemoryManager::RefillSlab(int32 block_size)
{
    void* slab_base = _slab_pools[block_size]->Pop();

    SlabInfo* slab_info = xnew<SlabInfo>(slab_base, block_size, CalcBlockCountOfSlab(block_size));
    SlabMetadata::InitHeader(slab_base, slab_info);

    return slab_info;
}

void SlabMemoryManager::DrainSlab(void* slab_base)
{
    SlabMetadata* header = reinterpret_cast<SlabMetadata*>(slab_base);
    int32 block_size = header->slab_info->alloc_size;    
    ASSERT_CRASH(block_size > 0);
    SlabInfo* slab_info = header->slab_info;

    xdelete<SlabInfo>(slab_info);
    slab_info = nullptr;
    _slab_pools[block_size]->Push(header);
}

void* SlabMemoryManager::AcquireBlock(int32 size)
{
    int32 alloc_size = size + sizeof(SlabMetadata);
    SlabInfo* slab_info = xnew<SlabInfo>(nullptr, alloc_size, 0);
    return SlabMetadata::InitHeader(_aligned_malloc(alloc_size, kMallocAlignment), slab_info);
}

void SlabMemoryManager::ReleaseBlock(SlabMetadata* header)
{
    xdelete<SlabInfo>(header->slab_info);
    header->slab_info = nullptr;
    _aligned_free(header);
}
