#include "pch.h"
#include "SlabMemoryManager.h"
#include "ChunkPool.h"

const int32 kMallocAlignment = 64;

// ------- SizeConverter ------- //
SizeConverter::SizeConverter()
    :_size_converter{}
{
    generate(arr_size.begin(), arr_size.end(), []() { static int32 seed; return seed += 32; });

    int32 size = 1;
    int32 index = 0;
    for (int32 i = kChunkStride; i <= kMaxBlockSize; i+= kChunkStride)
    {
        while (size <= i)
            _size_converter[size++] = index;
        ++index;
    }
}

// ------- ThreadLocalSlab ------- //
ThreadLocalSlab::ThreadLocalSlab(int32 alloc_size)
    : _block_size(alloc_size),
    _block_count(CalcBlockCountOfChunk(alloc_size))
{    
    _chunk_infos.reserve(kMaxChunkInfoCount);

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
    for (int32 i = 0; i < _chunk_infos.size();/*none*/)
    {
        if (_chunk_infos[i]->block_count == _chunk_infos[i]->freed_count.load(memory_order_acquire))
        {
            Flush(_chunk_infos[i]);

            _chunk_infos[i] = _chunk_infos.back();
            _chunk_infos.pop_back();
        }
        else
        {
            ++i;
        }
    }
}

void ThreadLocalSlab::Flush(ChunkInfo* chunk_info)
{    
    SlabMemoryManager::Instance().DrainChunk(chunk_info->chunk_base);
}

void ThreadLocalSlab::Fetch()
{
    ChunkInfo* chunk_info = SlabMemoryManager::Instance().RefillChunk(_block_size);
    
    _chunk_infos.push_back(chunk_info);

    int8* chunk_payload = reinterpret_cast<int8*>(ChunkHeader::DetachPayload(chunk_info));

    for (int32 i = 0; i < _block_count; ++i)
    {
        void* memory = reinterpret_cast<void*>(chunk_payload + (i * _block_size));
        void* block = BlockHeader::InitHeader(memory, chunk_info);

        _free_block_payloads.push_back(block);
    }
}

// ------- SlabMemoryManager ------- //
SlabMemoryManager::SlabMemoryManager()
{
    int32 size = 1;
    for (int32 block = 32; block <= kMaxBlockSize; block += kChunkStride)
    {
        ChunkPool* pool = new ChunkPool(block);
        while (size <= block)
            _chunk_pools[size++] = pool;
    }
}

SlabMemoryManager::~SlabMemoryManager()
{
    for (int32 block = 32; block <= kMaxBlockSize; block += kChunkStride)
        delete _chunk_pools[block];
}

void* SlabMemoryManager::Acquire(int32 size)    
{
    int32 alloc_size = size + sizeof(BlockHeader);
    if (alloc_size > kMaxBlockSize)
        return AcquireBlock(alloc_size);
    
    int32 index = SizeConverter::Instance().ToIndex(alloc_size);
    return tls_slab[index]->Acquire();
}

void SlabMemoryManager::Release(void* block_payload)
{
    BlockHeader* header = BlockHeader::DetachHeader(block_payload);
    auto& chunk_info = header->chunk_info;
    int32 alloc_size = chunk_info->alloc_size;
    ASSERT_CRASH(alloc_size > 0);

    if (alloc_size > kMaxBlockSize)
        ReleaseBlock(header);
    else
    {        
        chunk_info->freed_count.fetch_add(1, memory_order_acq_rel);
        int32 index = SizeConverter::Instance().ToIndex(chunk_info->alloc_size);
        tls_slab[index]->Release();
    }
}

ChunkInfo* SlabMemoryManager::RefillChunk(int32 block_size)
{
    void* chunk_base = _chunk_pools[block_size]->Pop();

    ChunkInfo* chunk_info = xnew<ChunkInfo>(chunk_base, block_size, CalcBlockCountOfChunk(block_size));
    ChunkHeader::InitHeader(chunk_base, chunk_info);

    return chunk_info;
}

void SlabMemoryManager::DrainChunk(void* chunk_base)
{
    ChunkHeader* header = reinterpret_cast<ChunkHeader*>(chunk_base);
    int32 block_size = header->chunk_info->alloc_size;    
    ASSERT_CRASH(block_size > 0);
    ChunkInfo* chunk_info = header->chunk_info;
    //{
    //    WRITE_LOCK;
    //    _free_ids.push_back(chunk_info->chunk_id);
    //}

    xdelete<ChunkInfo>(chunk_info);
    chunk_info = nullptr;
    _chunk_pools[block_size]->Push(header);
}

void* SlabMemoryManager::AcquireBlock(int32 size)
{
    int32 alloc_size = size + sizeof(BlockHeader);
    ChunkInfo* chunk_info = xnew<ChunkInfo>(nullptr, alloc_size, 0);
    return BlockHeader::InitHeader(_aligned_malloc(alloc_size, kMallocAlignment), chunk_info);
}

void SlabMemoryManager::ReleaseBlock(BlockHeader* header)
{
    xdelete<ChunkInfo>(header->chunk_info);
    header->chunk_info = nullptr;
    _aligned_free(header);
}
