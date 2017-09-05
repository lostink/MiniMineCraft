#ifndef CHUNK_H
#define CHUNK_H
#include <drawable.h>
#include <QList>
#include <scene\block.h>
#include <proceduralterrain.h>



class Chunk : public Drawable
{
private:
    QList<QList<QList<Block>>> blocks; //Store blocks info (active/inactive, block types)
    glm::vec4 startPos; //Bottom Left Position of the chunk
    bool visible;
    static const int CHUNK_SIZE = 16; //Each chunk has 16*16*16 blocks
    biometype chunkBiome;


public:
    Chunk(GLWidget277* context);
    //~Chunk();

    virtual void create();

    void createChunk(std::map<std::tuple<int, int, int>, blocktype> &blockInfo, std::tuple<int, int, int> startPos,
                     bool PosXBiome, bool NegXBiome, bool PosZBiome, bool NegZBiome);

    glm::vec4 getStartPos();

    void setStartPos(glm::vec4 start);

    bool isChunkVisible();

    void setChunkVisible(bool chunkVisible);

    void deleteBlockAt(int x, int y, int z);

    void addBlockAt(int x, int y, int z, blocktype bType);

    void setChunkBiome(biometype biomeType);

    biometype getChunkBiome();
};

#endif // CHUNK_H
