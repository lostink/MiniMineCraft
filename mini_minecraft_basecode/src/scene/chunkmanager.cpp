#include "chunkmanager.h"

ChunkManager::ChunkManager()
{
    chunkLists.clear();
}

ChunkManager::~ChunkManager(){
    for(unsigned int i=0; i<chunkLists.size(); i++){
        delete chunkLists[i];
        chunkLists[i] = nullptr;
    }
    //chunkLists.clear();
    //newChunks.clear();
}

void ChunkManager::createNewChunk(Chunk *chunk){
    chunkLists.push_back(chunk);
    newChunks.push_back(chunk);
}

std::vector<Chunk *> ChunkManager::getNewChunks()const{
    return newChunks;
}

void ChunkManager::clearNewChunks(){
    newChunks.clear();
}

std::vector<Chunk *> ChunkManager::getChunks()const{
    return chunkLists;
}

void ChunkManager::deleteBlockAt(int x, int y, int z){
    //Find which chunk is the block at
    Chunk* targetChunk = nullptr;
    for(unsigned int i=0 ;i<chunkLists.size(); i++){
        if(x>=chunkLists[i]->getStartPos()[0] && x<=chunkLists[i]->getStartPos()[0]+CHUNK_SIZE){
            if(y>=chunkLists[i]->getStartPos()[1] && y<=chunkLists[i]->getStartPos()[1]+CHUNK_SIZE){
                if(z>=chunkLists[i]->getStartPos()[2] && z<=chunkLists[i]->getStartPos()[2]+CHUNK_SIZE){
                    targetChunk = chunkLists[i];
                    break;
                }
            }
        }
    }

    glm::vec4 startPos = targetChunk->getStartPos();
    targetChunk->deleteBlockAt(x-startPos[0],y-startPos[1], z-startPos[2]);
    updatedChunks.push_back(targetChunk);
}

std:: vector<Chunk *> ChunkManager::getUpdatedChunks()const{
    return updatedChunks;
}

void ChunkManager::clearUpdatedChunks(){
    updatedChunks.clear();
}

void ChunkManager::addBlockAt(int x, int y, int z){
    //Find which chunk is the block at
    Chunk* targetChunk = nullptr;
    for(unsigned int i=0 ;i<chunkLists.size(); i++){
        if(x>=chunkLists[i]->getStartPos()[0] && x<=chunkLists[i]->getStartPos()[0]+CHUNK_SIZE){
            if(y>=chunkLists[i]->getStartPos()[1] && y<=chunkLists[i]->getStartPos()[1]+CHUNK_SIZE){
                if(z>=chunkLists[i]->getStartPos()[2] && z<=chunkLists[i]->getStartPos()[2]+CHUNK_SIZE){
                    targetChunk = chunkLists[i];
                    break;
                }
            }
        }
    }

    glm::vec4 startPos = targetChunk->getStartPos();
    targetChunk->addBlockAt(x-startPos[0],y-startPos[1], z-startPos[2]);
    updatedChunks.push_back(targetChunk);
}
