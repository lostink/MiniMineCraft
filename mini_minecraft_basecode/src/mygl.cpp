#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cube(this),geom_Center(this),chunkManager(),
      prog_lambert(this), prog_flat(this),
      gl_camera(),timeCount(0)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    //Lostink Test code
    geom_Center.setCenter(QPoint(width() / 2, height() / 2));
    ShowMouse = true;
    setMouseTracking(true);
    QCursor c = cursor();
    c.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
    setCursor(c);
    flag_amount_speed = 5.0 / 60.0;
    flag_moving_forward = 0;
    flag_moving_right = 0;
    flag_rotate_right = 0;
    //Test end.
    terrain.createInitialWorld();

    Tester.SetMainCamera(&gl_camera);
    Tester.SetMesh(&terrain.mapWorld);
    Tester.SetManager(&chunkManager);
    Tester.SetTerrain(&terrain);

    std::tuple<int, int, int> startPos(0,0,0);
    for(int x=0; x<4; x++){
        for(int y=-8; y<4; y++){
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
    geom_cube.create();
    geom_Center.create();
    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));

    //Yuxin MM02
    prog_lambert.setUpTexture();
    prog_lambert.setUpNormalMap();

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
        //std::cout<<"chunk is updated"<<std::endl;
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
    if(int(round(Tester.eye[0]))%16==0 || int(round(Tester.eye[2]))%16==0){
        chunkManager.checkVisibility(Tester.eye);
    }
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
    //YuxinMM02 activate the Texture and Normal map before rendering world scene
    prog_lambert.bindTexture0();
    prog_lambert.bindNormalMap0();
    prog_lambert.setEyePosition(Tester.eye);
    GLRenderWorld();
    prog_flat.setModelMatrix(glm::mat4(1.0));
    prog_flat.setViewProjMatrix(glm::mat4(1.0));
    prog_flat.draw(geom_Center);
}

//=====================Added by Yuxin==============================//
void MyGL:: GLRenderWorld()
{
    //Set the model matrix
    //glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0, 0, 32));
    glm::mat4 model = glm::mat4(1.0);
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

    float amount = 0.3f;
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
//        gl_camera.RotateAboutUp(-amount);
        Tester.RotateAboutUp(-amount);
        flag_rotate_right = 1;
    } else if (e->key() == Qt::Key_Left) {
//        gl_camera.RotateAboutUp(amount);
        Tester.RotateAboutUp(amount);
        flag_rotate_left = 1;
    } else if (e->key() == Qt::Key_Up) {
//        gl_camera.RotateAboutRight(-amount);
        Tester.RotateAboutRight(-amount);
        flag_rotate_up = 1;
    } else if (e->key() == Qt::Key_Down) {
//        gl_camera.RotateAboutRight(amount);
        Tester.RotateAboutRight(amount);
        flag_rotate_down = 1;
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
//        Tester.CheckTranslateAlongLook(amount);
//        NewChunk();
        flag_moving_forward = 1;
    } else if (e->key() == Qt::Key_S) {
//        Tester.CheckTranslateAlongLook(-amount);
//        NewChunk();
        flag_moving_backward = 1;
    } else if (e->key() == Qt::Key_D) {
        //gl_camera.TranslateAlongRight(amount);
        //Tester.CheckTranslateAlongRight(amount);
        //NewChunk();
        flag_moving_right = 1;
    } else if (e->key() == Qt::Key_A) {
//        gl_camera.TranslateAlongRight(-amount);
//        Tester.CheckTranslateAlongRight(-amount);
//        NewChunk();
        flag_moving_left = 1;
    } else if (e->key() == Qt::Key_Q) {
        if (Tester.DisableFlyingAndCollision == 1)
            flag_moving_down = 1;
    } else if (e->key() == Qt::Key_E) {
//        gl_camera.TranslateAlongUp(amount);
        if (Tester.DisableFlyingAndCollision == 1)
            flag_moving_up = 1;
    } else if (e->key() == Qt::Key_R) {
//        gl_camera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_Space){
        Tester.Jump();
    }else if (e->key() == Qt::Key_F3)
    {
        ShowMouse^=1;
    }else if (e->key() == Qt::Key_F)
    {
        Tester.ChangeFlyingAndCollision();
    }
    //tst end
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
//Lostink insert code
void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_W){
        flag_moving_forward = 0;
    }else if (e->key() == Qt::Key_S) {
        flag_moving_backward = 0;
    } else if (e->key() == Qt::Key_D) {
        flag_moving_right = 0;
    } else if (e->key() == Qt::Key_A) {
        flag_moving_left = 0;
    } else if (e->key() == Qt::Key_Right) {
        flag_rotate_right = 0;
    } else if (e->key() == Qt::Key_Left) {
        flag_rotate_left = 0;
    } else if (e->key() == Qt::Key_Up) {
        flag_rotate_up = 0;
    } else if (e->key() == Qt::Key_Down) {
        flag_rotate_down = 0;
    } else if (e->key() == Qt::Key_Q){
        flag_moving_down = 0;
    } else if (e->key() == Qt::Key_E){
        flag_moving_up = 0;
    }
}
void MyGL::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {Tester.AddBlockToLookAt();}
    else if (e->button() == Qt::LeftButton)
    {Tester.DeleteBlockLookAt();}
}
void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    //printf("Moving!\n");
    if (ShowMouse == false) return;
    double speed = 20;
    QCursor c = cursor();
    QPoint Now = e->pos();
    float dx = Now.x() - (width() / 2);
    float dy = Now.y() - (height() / 2);
    //printf("%d %d\n",Now.y(),height() / 2);
    float upDeg = speed * dx / width();
    float rightDeg = speed  * dy / height();
    Tester.CheckRotateAboutUp(-upDeg);
    Tester.CheckRotateAboutRight(-rightDeg);
    c.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
    c.setShape(Qt::BlankCursor);
    setCursor(c);
}
void MyGL::Moving(){
    if (flag_moving_forward && flag_moving_backward)
    {}
    else if (flag_moving_forward)
    {
        Tester.CheckTranslateAlongLook(flag_amount_speed);
        NewChunk();
    }
    else if (flag_moving_backward)
    {
        Tester.CheckTranslateAlongLook(-flag_amount_speed);
        NewChunk();
    }
    if (flag_moving_left && flag_moving_right)
    {}
    else if (flag_moving_left)
    {
        Tester.CheckTranslateAlongRight(-flag_amount_speed);
        NewChunk();
    }
    else if (flag_moving_right)
    {
        Tester.CheckTranslateAlongRight(flag_amount_speed);
        NewChunk();
    }
    if (flag_moving_down && flag_moving_up)
    {}
    else if (flag_moving_down)
    {
        Tester.CheckTranslateAlongUp(-flag_amount_speed);
    }
    else if (flag_moving_up)
    {
        Tester.CheckTranslateAlongUp(flag_amount_speed);
    }
}
//Insert End.
void MyGL::deleteBlock(int x, int y, int z){
    chunkManager.deleteBlockAt(x, y, z);
}

void MyGL::addBlock(int x, int y, int z){
    chunkManager.addBlockAt(x, y, z);
}
void MyGL::timerUpdate()
{
    timeCount++;
    //pass this value to fragement shader to make water and lava shader moving
    if(timeCount==128){
        timeCount = 0;
    }
    if(timeCount%8==0){
        prog_lambert.setTimeCount(timeCount/8);
        update();
    }
    Tester.Falling();
    update();
    Moving();
}
void MyGL::NewChunk(){
    vector<tuple<int,int,int>>* NC= Tester.GetNewBlockVec();
    int total = NC->size();
    if (total == 0) return;
    for (int i=0;i<total;++i)
        createNewChunk(terrain.mapWorld,(*NC)[i]);
    (*NC).clear();
}
