#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H
#include <scene/chunk.h>


class ChunkManager
{
private:
    std::vector<Chunk*> chunkLists;//Store full chunks
    std::vector<Chunk*> updatedChunks;//Store chunks that were updated during last frame
    std::vector<Chunk*> newChunks;//Store Chunks that were newly created during last frame
    static const int CHUNK_SIZE = 16; //Each chunk has 16*16*16 blocks

public:
    ChunkManager();
    ~ChunkManager();
    void createNewChunk(Chunk* chunk);
    std::vector<Chunk*> getChunks() const;

    std::vector<Chunk*> getNewChunks() const;
    void clearNewChunks();

    void deleteBlockAt(int x, int y, int z);
    std::vector<Chunk*> getUpdatedChunks() const;
    void clearUpdatedChunks();

    void addBlockAt(int x, int y, int z);
};

#endif // CHUNKMANAGER_H
