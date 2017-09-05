#ifndef BLOCK_H
#define BLOCK_H
#include <proceduralterrain.h>

enum facetype{side = 0, top = 1, bottom = 2};

class Block
{
private:
    bool active;
    blocktype btype;
    biometype biomeType;

public:
    Block();
    bool isBlockActive();
    void setBlockActive(bool blockActive);
    blocktype getBlockType();
    void setBlockType(blocktype type);
    biometype getBiomType();
    void setBiomeType(biometype biome);
};

#endif // BLOCK_H
