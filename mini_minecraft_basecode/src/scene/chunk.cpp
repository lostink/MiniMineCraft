#include "chunk.h"
#include <iostream>

Chunk::Chunk(GLWidget277 *context):Drawable(context),visible(true),startPos(glm::vec4(0,0,0,1)){

}

glm::vec4 Chunk::getStartPos(){
    return startPos;
}

void Chunk::setStartPos(glm::vec4 start){
    startPos = start;
}

void Chunk::createChunk(std::map<std::tuple<int, int, int>,int> &blockInfo, std::tuple<int, int, int> startPos){
    //Pop in all blocks in chunk
    //Question better to use Array?? Since it takes 3 iterations to initialize blocks
    for(int i=0; i<CHUNK_SIZE;i++){
        QList<QList<Block>> xBlocks;

        for(int j=0; j<CHUNK_SIZE;j++){

            QList<Block> yBlocks;
            for(int k=0; k<CHUNK_SIZE;k++){
                yBlocks.push_back(Block());
            }

            xBlocks.push_back(yBlocks);
        }
        blocks.push_back(xBlocks);
    }


    /*for(unsigned int index=0; index<blockInfo.size(); index++){
        int i = std::get<0>(blockInfo[index]); //i
        int j = std::get<1>(blockInfo[index]); //j
        int k = std::get<2>(blockInfo[index]); //k
        blocks[i][j][k].setBlockActive(true);
    }*/


    //Start from startPos, check if the map key exists
    int startX = std::get<0>(startPos);
    int startY = std::get<1>(startPos);
    int startZ = std::get<2>(startPos);

    for(int x = startX; x< startX+CHUNK_SIZE; x++){
        for(int y=startY; y<startY+CHUNK_SIZE;y++){
            for(int z = startZ; z<startZ+CHUNK_SIZE;z++){
                std::tuple<int, int, int> key(x,y,z);
                //If this block exists in the terrain
                if (blockInfo.find(key) != blockInfo.end()){
                    blocks[x-startX][y-startY][z-startZ].setBlockActive(true);
                }
            }
        }
    }
}

void createCubeVertAttribute(std::vector<glm::vec4>& pos,std::vector<glm::vec4>& nor,std::vector<glm::vec4>& col,
                        int i, int j, int k,bool negativeX, bool positiveX,
                        bool negativeY, bool positiveY, bool negativeZ, bool positiveZ){
    //Do not render left face if negativeX is true
    if(!negativeX){
        pos.push_back(glm::vec4(i,j+1,k,1));
        pos.push_back(glm::vec4(i,j+1,k-1,1));
        pos.push_back(glm::vec4(i,j,k-1,1));
        pos.push_back(glm::vec4(i,j,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(-1,0,0,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
    }
    //Right Face
    if(!positiveX){
        //Right Side Face
        pos.push_back(glm::vec4(i+1,j+1,k,1));
        pos.push_back(glm::vec4(i+1,j,k,1));
        pos.push_back(glm::vec4(i+1,j,k-1,1));
        pos.push_back(glm::vec4(i+1,j+1,k-1,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(1,0,0,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
    }
    //Bottom Face
    if(!negativeY){
        pos.push_back(glm::vec4(i,j,k,1));
        pos.push_back(glm::vec4(i,j,k-1,1));
        pos.push_back(glm::vec4(i+1,j,k-1,1));
        pos.push_back(glm::vec4(i+1,j,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,-1,0,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
    }
    //Top Face
    if(!positiveY){
        pos.push_back(glm::vec4(i+1,j+1,k,1));
        pos.push_back(glm::vec4(i+1,j+1,k-1,1));
        pos.push_back(glm::vec4(i,j+1,k-1,1));
        pos.push_back(glm::vec4(i,j+1,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,1,0,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
    }
    //Back Face
    if(!negativeZ){
        pos.push_back(glm::vec4(i,j,k-1,1));
        pos.push_back(glm::vec4(i+1,j,k-1,1));
        pos.push_back(glm::vec4(i+1,j+1,k-1,1));
        pos.push_back(glm::vec4(i,j+1,k-1,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,0,-1,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
    }
    //Front Face
    if(!positiveZ){
        pos.push_back(glm::vec4(i,j,k,1));
        pos.push_back(glm::vec4(i+1,j,k,1));
        pos.push_back(glm::vec4(i+1,j+1,k,1));
        pos.push_back(glm::vec4(i,j+1,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,0,1,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
    }

}

void createCube(std::vector<glm::vec4>& chunk_vert, int i, int j, int k, bool negativeX, bool positiveX,
                bool negativeY, bool positiveY, bool negativeZ, bool positiveZ){
    //i,j,k represents the bottom left vert position
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<glm::vec4> nor;
    createCubeVertAttribute(pos,nor,col,i,j,k,negativeX,positiveX,negativeY,positiveY,negativeZ,positiveZ);
    for(unsigned int index = 0; index < pos.size(); index++){
        //(V[0]V[1]V[2]V[3]N[0]N[1]N[2]N[3]C[0]C[1]C[2]C[3])
        chunk_vert.push_back(pos[index]);
        chunk_vert.push_back(nor[index]);
        chunk_vert.push_back(col[index]);
    }
}

void createCubeIndex(std::vector<GLuint>& chunk_idx, int numOfFaces){
    for(int i=0; i<numOfFaces; i++){
        chunk_idx.push_back(i*4);
        chunk_idx.push_back(i*4+1);
        chunk_idx.push_back(i*4+2);
        chunk_idx.push_back(i*4);
        chunk_idx.push_back(i*4+2);
        chunk_idx.push_back(i*4+3);
    }
}

void createChunkVerts(std::vector<glm::vec4>& chunk_vert, std::vector<GLuint>& chunk_idx,int chunkSize,
                      QList<QList<QList<Block>>>& blocks, glm::vec4 startPos){
    //int startIndex = 0;
    bool negativeX, positiveX, negativeY, positiveY, negativeZ, positiveZ;
    for(int i=0; i<chunkSize; i++){
        for(int j=0; j<chunkSize; j++){
            for(int k=0; k<chunkSize; k++){
                if(blocks[i][j][k].isBlockActive()){
                    //For each vertex, the info stored in chunk_vert is (VNCVNCVNCVNC)
                    //check -X
                    if(i>0){
                        negativeX = blocks[i-1][j][k].isBlockActive();
                    }else{
                        negativeX = false;
                    }
                    //check +X
                    if(i<chunkSize-1){
                        positiveX =  blocks[i+1][j][k].isBlockActive();
                    }else{
                        positiveX = false;
                    }
                    //check -Y
                    if(j>0){
                        negativeY = blocks[i][j-1][k].isBlockActive();
                    }else{
                        negativeY = false;
                    }
                    //check +Y
                    if(j<chunkSize-1){
                        positiveY = blocks[i][j+1][k].isBlockActive();
                    }else{
                        positiveY = false;
                    }
                    //check -Z
                    if(k>0){
                        negativeZ = blocks[i][j][k-1].isBlockActive();
                    }else{
                        negativeZ = false;
                    }
                    //check +Z
                    if(k<chunkSize-1){
                        positiveZ = blocks[i][j][k+1].isBlockActive();
                    }else{
                        positiveZ = false;
                    }
                    createCube(chunk_vert, startPos[0]+i,startPos[1]+j,startPos[2]+k,negativeX,positiveX, negativeY,
                            positiveY, negativeZ, positiveZ);
                    createCubeIndex(chunk_idx, chunk_vert.size()/12);
                }
            }
        }
    }
}

void Chunk::create(){
    std::vector<GLuint> chunk_idx;
    std::vector<glm::vec4> chunk_vert;
    createChunkVerts(chunk_vert,chunk_idx,CHUNK_SIZE, blocks,startPos);


    int CHUNK_IDX_COUNT = chunk_idx.size();
    int CHUNK_VERT_COUNT = chunk_vert.size();//CHUNK_VERT_COUNT is actually 3 times of the actual vertex number

    count = CHUNK_IDX_COUNT;

    //For Debug Purpose Only
    /*for(int i=0 ;i<chunk_vert.size(); i++){
        std::cout<<"chunk_vert "<<i<<" is "<<glm::to_string(chunk_vert[i])<<std::endl;
    }

    for(int i=0 ;i<chunk_idx.size(); i++){
        std::cout<<"chunk_idx "<<i<<" is "<<chunk_idx[i]<<std::endl;
    }*/

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, CHUNK_IDX_COUNT * sizeof(GLuint), chunk_idx.data(), GL_STATIC_DRAW);

    generateChunk();
    context->glBindBuffer(GL_ARRAY_BUFFER,bufChunk);
    context->glBufferData(GL_ARRAY_BUFFER, CHUNK_VERT_COUNT*sizeof(glm::vec4),chunk_vert.data(), GL_STATIC_DRAW);
}

void Chunk::setChunkVisible(bool chunkVisible){
    visible = chunkVisible;
}

bool Chunk::isChunkVisible(){
    return visible;
}

void Chunk::deleteBlockAt(int x, int y, int z){
    blocks[x][y][z].setBlockActive(false);
}

void Chunk::addBlockAt(int x, int y, int z){
    blocks[x][y][z].setBlockActive(true);
}
