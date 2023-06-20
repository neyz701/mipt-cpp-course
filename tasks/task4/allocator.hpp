#pragma once

// When chunk is free 'next' contains pointer to a next free chunk
// When it's allocated, this space is used by user
struct Chunk {
    Chunk* next;
};

class PoolAllocator {
public:
    PoolAllocator(size_t chunksPerBlock) : chunksPerBlock_(chunksPerBlock), alloc_(nullptr) { }

    void* allocate(size_t size);

    void deallocate(void* chunk, size_t size);

private:
    size_t chunksPerBlock_;
    Chunk* alloc_;

    Chunk* allocateBlock(size_t chunkSize);
};

Chunk* PoolAllocator::allocateBlock(size_t chunkSize) {
    size_t blockSize = chunksPerBlock_ * chunkSize;

    Chunk* blockBegin = reinterpret_cast<Chunk*>(malloc(blockSize));
    Chunk* chunk = blockBegin;

    for (int i = 0; i < chunksPerBlock_ - 1; ++i) {
        chunk->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
        chunk = chunk->next;
    }

    chunk->next = nullptr;
    return blockBegin;
}

void* PoolAllocator::allocate(size_t size) {
    if (alloc_ == nullptr) {
        alloc_ = allocateBlock(size);
    }
    Chunk* freeChunk = alloc_;
    alloc_ = alloc_->next;
    return freeChunk;
}

void PoolAllocator::deallocate(void* chunk, size_t size) {
    reinterpret_cast<Chunk*>(chunk)->next = alloc_;
    alloc_ = reinterpret_cast<Chunk*>(chunk);
}