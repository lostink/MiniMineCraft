#include "block.h"

Block::Block():active(false)
{

}

bool Block::isBlockActive(){
    return active;
}

void Block::setBlockActive(bool blockActive){
    active = blockActive;
}

blocktype Block::getBlockType(){
    return btype;
}

void Block::setBlockType(blocktype type){
    btype = type;
}

biometype Block::getBiomType(){
    return biomeType;
}

void Block::setBiomeType(biometype biome){
    biomeType = biome;
}
