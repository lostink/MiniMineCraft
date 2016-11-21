#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),chunkManager(),
      prog_lambert(this), prog_flat(this),
      gl_camera(),timeCount(0)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    terrain.createInitialWorld();

    std::tuple<int, int, int> startPos(0,0,0);
    for(int x=0; x<4; x++){
        for(int y=0; y<4; y++){
            for(int z=0; z<4; z++){
                startPos = std::tuple<int, int, int>(x*16,y*16,z*16);
                createNewChunk(terrain.mapWorld,startPos);
            }
        }
    }
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    //geom_cube.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of Cube
    //geom_cube.create();

    //=====================Added By Yuxin For Testing============================//
    /*std::vector<std::tuple<int,int,int>> blockInfo;
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            for(int k=0; k<16; k++){
                std::tuple<int,int, int> testBlock (i,j,k);
                blockInfo.push_back(testBlock);
            }
        }
    }
    std::tuple<int, int, int> startPos(0,0,0);
    //chunk.createChunk(blockInfo);
    //chunk.create();
    createNewChunk(blockInfo,startPos);*/

    /*std::map<std::tuple<int, int, int>,blocktype> blockInfo;
    for(int i=0; i<16; i++){
        for(int j=0; j<8; j++){
            for(int k=0; k<16; k++){
                std::tuple<int,int, int> testBlock (i,j,k);
                blockInfo.insert(std::pair<std::tuple<int, int, int>, blocktype>(testBlock,dirt));
            }
        }
    }
    std::tuple<int, int, int> startPos(0,0,0);
    createNewChunk(blockInfo,startPos);*/

    //=====================Added By Yuxin For Testing============================//

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

    scene.CreateTestScene();
}

//===========================Added By Yuxin===============================//
void MyGL::createNewChunk(std::map<std::tuple<int, int, int>,blocktype>& blockInfo,std::tuple<int, int, int> startPos){
    Chunk* chunk = new Chunk(this);
    chunk->createChunk(blockInfo,startPos);
    chunk->setStartPos(glm::vec4(std::get<0>(startPos), std::get<1>(startPos), std::get<2>(startPos),1));
    //chunk.create();
    chunkManager.createNewChunk(chunk);
}
//===========================Added By Yuxin===============================//

//===========================Added By Yuxin===============================//
void MyGL::updateChunkVBO(){
    //****************Create VBOs for the new Chunks****************//
    Chunk* newChunk = nullptr;
    for(unsigned int i=0; i<chunkManager.getNewChunks().size(); i++){
        newChunk = chunkManager.getNewChunks()[i];
        //Create VBO data for the newly generated chunks
        newChunk->create();
    }
    chunkManager.clearNewChunks();
    //****************Create VBOs for the chunks that have been changed (deleted/added)*****************//
    Chunk* updateChunk = nullptr;
    for(unsigned int i=0; i<chunkManager.getUpdatedChunks().size();i++){
        std::cout<<"chunk is updated"<<std::endl;
        updateChunk = chunkManager.getUpdatedChunks()[i];
        //Create VBO data for the updated chunks
        updateChunk->create();
    }
    chunkManager.clearUpdatedChunks();
}

//===========================Added By Yuxin===============================//

//===========================Added By Yuxin===============================//
void MyGL::updateChunkVisibility(){
    //get the player position, if the player is at the edge of the chunk
    if(int(round(Tester.eye[0]))%16==0 && int(round(Tester.eye[1]))%16==0 && int(round(Tester.eye[2]))%16==0){
        chunkManager.checkVisibility(Tester.eye);
    }
    //pass the player position to the chunkManager
}

//===========================Added By Yuxin===============================//

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
//    gl_camera = Camera(w, h);
    gl_camera = Camera(w, h, glm::vec3(scene.dimensions.x/2, scene.dimensions.y/2 + 2, scene.dimensions.z/2),
                       glm::vec3(scene.dimensions.x/2, scene.dimensions.y/2+2, scene.dimensions.z/2+1), glm::vec3(0,1,0));
    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
//DO NOT CONSTRUCT YOUR SCENE GRAPH IN THIS FUNCTION!
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());

    //GLDrawScene();
    GLRenderWorld();
}

//=====================Added by Yuxin==============================//
void MyGL:: GLRenderWorld()
{
    //Set the model matrix
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0, 0, 32));
    prog_lambert.setModelMatrix(model);
    //prog_lambert.draw(chunk);

    //**************Update Chunk VBO*****************//
    updateChunkVBO();

    //**************Update chunk visibility***********//
    //if the player position is at the edge of the new chunk, check all chunks visibility
    updateChunkVisibility();

    //***********Draw world chunks**************//
    for(unsigned int i=0; i<chunkManager.getChunks().size(); i++){
        if(chunkManager.getChunks()[i]->isChunkVisible()){
            prog_lambert.draw(*(chunkManager.getChunks()[i]));
        }
    }
}
//=====================Added by Yuxin==============================//


/*void MyGL::GLDrawScene()
{
    for(int x = 0; x < 64; x++)
    {
        QList<QList<bool>> Xs = scene.objects[x];
        for(int y = 0; y < 64; y++)
        {
            QList<bool> Ys = Xs[y];
            //for(int z = 0; z < Ys.size(); z++)
            //QList<bool> Ys = Xs[x];
            for(int z = 0; z < 64; z++)
            {
                if(terrain.searchBlockAt(x, y, z))
                {
                    prog_lambert.setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, y, z)));
                    prog_lambert.draw(geom_cube);
                }
            }
        }
    }
}*/
void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 0.5f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
//        Tester.CheckTranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
//        Tester.CheckTranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        gl_camera = Camera(this->width(), this->height());
    }
    //Test code:
        else if (e->key() == Qt::Key_Space){
        Tester.Jump();
    }
    //tst end
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::deleteBlock(int x, int y, int z){
    chunkManager.deleteBlockAt(x, y, z);
}

void MyGL::addBlock(int x, int y, int z){
    chunkManager.addBlockAt(x, y, z);
}

void MyGL::timerUpdate()
{

    /*timeCount++;
    //Delete a block every 60 timeCounts
    int pos = 0;
    if(timeCount%60==0){
        std::cout<<"delete time count is: "<<timeCount<<std::endl;
        pos = timeCount/60;
        deleteBlock(pos, 0, 0);
        //addBlock(pos,0,0);
    }
    update();*/

    //Test code from Lostink
    //Testing physical system
        //printf("%lf %lf %lf\n",gl_camera.eye[0],gl_camera.eye[1],gl_camera.eye[2]);
        //tuple<int,int,int> a(32,20,32),b(32,21,31);
        //test[a] = 1;
        Tester.SetMainCamera(&gl_camera);
        Tester.SetMesh(&terrain.mapWorld);
        //Tester.Falling();
        update();
    //Test code end

}

