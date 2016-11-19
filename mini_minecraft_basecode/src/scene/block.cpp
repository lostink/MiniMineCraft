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
