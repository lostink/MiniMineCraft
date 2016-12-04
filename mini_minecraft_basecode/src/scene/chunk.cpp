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

void Chunk::createChunk(std::map<std::tuple<int, int, int>,blocktype> &blockInfo, std::tuple<int, int, int> startPos){
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
                    //Yuxin MM2
                    blocks[x-startX][y-startY][z-startZ].setBlockType(blockInfo.at(key));
                }
            }
        }
    }
}

//Yuxin MM02
void getBlockFaceUV(blocktype btype, facetype ftype, std::vector<glm::vec2>& uv){
    switch (btype)
    {
    case DIRT:
       // Add your code here
        if(ftype == side){
            uv.push_back(glm::vec2(2.0/16,0));
            uv.push_back(glm::vec2(3.0/16,0));
            uv.push_back(glm::vec2(3.0/16,1.0/16));
            uv.push_back(glm::vec2(2.0/16,1.0/16));
        }
        if(ftype == top){
            uv.push_back(glm::vec2(2.0/16,0));
            uv.push_back(glm::vec2(3.0/16,0));
            uv.push_back(glm::vec2(3.0/16,1.0/16));
            uv.push_back(glm::vec2(2.0/16,1.0/16));
        }
        if(ftype == bottom){
            uv.push_back(glm::vec2(2.0/16,0));
            uv.push_back(glm::vec2(3.0/16,0));
            uv.push_back(glm::vec2(3.0/16,1.0/16));
            uv.push_back(glm::vec2(2.0/16,1.0/16));

        }
        break;
    case GRASS:
       // Add code here
        if(ftype == side){
            uv.push_back(glm::vec2(3.0/16,0));
            uv.push_back(glm::vec2(4.0/16,0));
            uv.push_back(glm::vec2(4.0/16,1.0/16));
            uv.push_back(glm::vec2(3.0/16,1.0/16));
        }
        if(ftype == top){
            uv.push_back(glm::vec2(8.0/16,2.0/16));
            uv.push_back(glm::vec2(9.0/16,2.0/16));
            uv.push_back(glm::vec2(9.0/16,3.0/16));
            uv.push_back(glm::vec2(8.0/16,3.0/16));
        }
        if(ftype == bottom){
            uv.push_back(glm::vec2(2.0/16,0));
            uv.push_back(glm::vec2(3.0/16,0));
            uv.push_back(glm::vec2(3.0/16,1.0/16));
            uv.push_back(glm::vec2(2.0/16,1.0/16));

        }
        break;
    case LAVA:
        //Add code here
        uv.push_back(glm::vec2(13.0/16,14.0/16));
        uv.push_back(glm::vec2(14.0/16,14.0/16));
        uv.push_back(glm::vec2(14.0/16,15.0/16));
        uv.push_back(glm::vec2(13.0/16,15.0/16));
        break;
    case WATER:
        uv.push_back(glm::vec2(13.0/16,12.0/16));
        uv.push_back(glm::vec2(14.0/16,12.0/16));
        uv.push_back(glm::vec2(14.0/16,13.0/16));
        uv.push_back(glm::vec2(13.0/16,13.0/16));
        break;
    default:
        uv.push_back(glm::vec2(0,0));
        uv.push_back(glm::vec2(1.0/16,0));
        uv.push_back(glm::vec2(1.0/16,1.0/16));
        uv.push_back(glm::vec2(0,1.0/16));
    }

}

void getBlockFaceShiness(blocktype btype, std::vector<float>& shiness){
    switch(btype)
    {
    case DIRT:
        for(int i=0; i<4; i++){
            shiness.push_back(0.3);
        }
        break;
    case GRASS:
        for(int i=0; i<4; i++){
            shiness.push_back(0.1);
        }
        break;
    case LAVA:
        for(int i=0; i<4; i++){
            shiness.push_back(0.5);
        }
        break;
    case WATER:
        for(int i=0; i<4; i++){
            shiness.push_back(0.4);
        }
    default:
        for(int i=0; i<4; i++){
            shiness.push_back(0.5);
        }
    }
}

void getBlockFaceType(blocktype btype, std::vector<float>& blockType){
    if(btype == LAVA || btype == WATER){
        for(int i=0; i<4; i++){
            blockType.push_back(3.0);
        }

    }else{
        for(int i=0; i<4; i++){
           blockType.push_back(0.0);
        }
    }
}

void createCubeVertAttribute(std::vector<glm::vec4>& pos,std::vector<glm::vec4>& nor,std::vector<glm::vec4>& col,
                        int i, int j, int k,bool negativeX, bool positiveX, bool negativeY, bool positiveY,
                             bool negativeZ, bool positiveZ, std::vector<glm::vec2>& uv,
                             blocktype btype, std::vector<float>& shiness, std::vector<float>& blockType){
    //Do not render left face if negativeX is true
    if(!negativeX){
        pos.push_back(glm::vec4(i,j+1,k,1));
        pos.push_back(glm::vec4(i,j+1,k+1,1));
        pos.push_back(glm::vec4(i,j,k+1,1));
        pos.push_back(glm::vec4(i,j,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(-1,0,0,0));
            col.push_back(glm::vec4(0,1,0,1));
        }
        //Yuxin MM02
        getBlockFaceUV(btype,side,uv);
        getBlockFaceShiness(btype, shiness);
        getBlockFaceType(btype,blockType);
    }
    //Right Side Face
    if(!positiveX){        
        pos.push_back(glm::vec4(i+1,j+1,k+1,1));
        pos.push_back(glm::vec4(i+1,j+1,k,1));
        pos.push_back(glm::vec4(i+1,j,k,1));
        pos.push_back(glm::vec4(i+1,j,k+1,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(1,0,0,0));
            col.push_back(glm::vec4(1,0,0,1));
        }
        //Yuxin MM02
        getBlockFaceUV(btype,side,uv);
        getBlockFaceShiness(btype, shiness);
        getBlockFaceType(btype,blockType);
    }
    //Bottom Face
    if(!negativeY){
        pos.push_back(glm::vec4(i+1,j,k,1));
        pos.push_back(glm::vec4(i,j,k,1));
        pos.push_back(glm::vec4(i,j,k+1,1));
        pos.push_back(glm::vec4(i+1,j,k+1,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,-1,0,0));
            col.push_back(glm::vec4(0,0,1,1));
        }

        //Yuxin MM02
        getBlockFaceUV(btype,bottom,uv);
        getBlockFaceShiness(btype, shiness);
        getBlockFaceType(btype,blockType);
    }
    //Top Face
    if(!positiveY){
        pos.push_back(glm::vec4(i+1,j+1,k+1,1));
        pos.push_back(glm::vec4(i,j+1,k+1,1));
        pos.push_back(glm::vec4(i,j+1,k,1));
        pos.push_back(glm::vec4(i+1,j+1,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,1,0,0));
            col.push_back(glm::vec4(1,1,0,1));
        }

        //Yuxin MM02
        getBlockFaceUV(btype,top,uv);
        getBlockFaceShiness(btype, shiness);
        getBlockFaceType(btype,blockType);
    }
    //Front Face
    if(!negativeZ){
        pos.push_back(glm::vec4(i+1,j+1,k,1));
        pos.push_back(glm::vec4(i,j+1,k,1));
        pos.push_back(glm::vec4(i,j,k,1));
        pos.push_back(glm::vec4(i+1,j,k,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,0,-1,0));
            col.push_back(glm::vec4(0,1,1,1));
        }
        //Yuxin MM02
        getBlockFaceUV(btype,side,uv);
        getBlockFaceShiness(btype, shiness);
        getBlockFaceType(btype,blockType);
    }
    //Back Face
    if(!positiveZ){
        pos.push_back(glm::vec4(i,j+1,k+1,1));
        pos.push_back(glm::vec4(i+1,j+1,k+1,1));
        pos.push_back(glm::vec4(i+1,j,k+1,1));
        pos.push_back(glm::vec4(i,j,k+1,1));

        for(int i=0; i<4; i++){
            nor.push_back(glm::vec4(0,0,1,0));
            col.push_back(glm::vec4(1,0,1,1));
        }
        //Yuxin MM02
        getBlockFaceUV(btype,side,uv);
        getBlockFaceShiness(btype, shiness);
        getBlockFaceType(btype,blockType);
    }

}

void createCube(std::vector<glm::vec4>& chunk_vert, int i, int j, int k, bool negativeX, bool positiveX,
                bool negativeY, bool positiveY, bool negativeZ, bool positiveZ, blocktype btype){
    //i,j,k represents the bottom left vert position
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec2> uv;
    std::vector<float> blockType;
    std::vector<float> shiness;
    createCubeVertAttribute(pos,nor,col,i,j,k,negativeX,positiveX,negativeY,positiveY,negativeZ,positiveZ,
                            uv,btype,shiness,blockType);
    for(unsigned int index = 0; index < pos.size(); index++){
        //(V[0]V[1]V[2]V[3]N[0]N[1]N[2]N[3]C[0]C[1]C[2]C[3]UV00blockType000shiness000)
        chunk_vert.push_back(pos[index]);
        chunk_vert.push_back(nor[index]);
        chunk_vert.push_back(col[index]);
        chunk_vert.push_back(glm::vec4(uv[index][0],uv[index][1],0,0));
        chunk_vert.push_back(glm::vec4(blockType[index],0,0,0));
        chunk_vert.push_back(glm::vec4(shiness[index],0,0,0));
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
                            positiveY, negativeZ, positiveZ,blocks[i][j][k].getBlockType());
                    //createCubeIndex(chunk_idx, chunk_vert.size()/12);
                }
            }
        }
    }
    createCubeIndex(chunk_idx, chunk_vert.size()/12);
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

    chunk_idx.clear();
    chunk_vert.clear();
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
