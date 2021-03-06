#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cube(this),geom_Center(this),chunkManager(),
      prog_lambert(this), prog_flat(this),geom_Inventory(this),
      gl_camera(),timeCount(0),skyColor(glm::vec4(0, 0, 0, 1)),lightDir(glm::vec4(-1,0,1,0)),lightCol(glm::vec4(1,1,0,0)),
      enableDayNightCycle(true)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    //Lostink Test code
    geom_Center.setCenter(QPoint(width() / 2, height() / 2));
    geom_Inventory.setCenter(QPoint(width() / 2, height() / 2));
    ShowMouse = true;
    setMouseTracking(true);
    QCursor c = cursor();
    c.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
    setCursor(c);
    flag_amount_speed = 5.0 / 60.0;
    flag_moving_forward = 0;
    flag_moving_backward = 0;
    flag_moving_right = 0;
    flag_moving_left = 0;
    flag_moving_up = 0;      //Test mode only
    flag_moving_down = 0;    //Test mode only
    flag_rotate_right = 0;
    flag_rotate_left = 0;
    flag_rotate_up = 0;
    flag_rotate_down = 0;
    flag_rotate_right = 0;
    WalkingStep = nullptr;
    flag_walking = 0;
    //Test end.
    terrain.createInitialWorld();

    Tester.SetMainCamera(&gl_camera);
    Tester.SetMesh(&terrain.mapWorld);
    Tester.SetManager(&chunkManager);
    Tester.SetTerrain(&terrain);
    Tester.setCenter(&geom_Center);
    Tester.SetAudio(&audio);

    std::tuple<int, int, int> startPos(0,0,0);
    for(int x=0; x<4; x++){
        for(int y=-8; y<4; y++){
            for(int z=0; z<4; z++){
                startPos = std::tuple<int, int, int>(x*16,y*16,z*16);
                createNewChunk(terrain.mapWorld,startPos,terrain.mapBiome);
            }
        }
    }

    //audio.playFootstep(PLAIN, 5);
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
    //glClearColor(0.37f, 0.74f, 1.0f, 1);
    //glClearColor(skyColor[0], skyColor[1], skyColor[2], skyColor[3]);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of Cube
    geom_cube.create();
    geom_Center.create();
    geom_Inventory.create();
    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    prog_flat.setUpInventory();

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));

    //Yuxin MM02
    prog_lambert.setUpTexture();
    prog_lambert.setUpNormalMap();
    //Yuxin MM03
    prog_lambert.setUpGrayTexture();
    //Yuxin MM03 Enable ALPHA channel
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    // vao.bind();
    glBindVertexArray(vao);

    scene.CreateTestScene();
}

//===========================Added By Yuxin===============================//
void MyGL::createNewChunk(std::map<std::tuple<int, int, int>,blocktype>& blockInfo, std::tuple<int, int, int> startPos,
                          std::map<std::pair<int, int>, biometype> &biomeInfo){
    Chunk* chunk = new Chunk(this);
    //Check neighboring chunk biome type
    glm::vec4 chunkStartPos = glm::vec4(std::get<0>(startPos), std::get<1>(startPos), std::get<2>(startPos),1);
    Chunk* neighborChunk = nullptr;
    bool posXBiome=false;
    bool negXBiome=false;
    bool posZBiome=false;
    bool negZBiome=false;
    std::pair<int, int>biomeKey(chunkStartPos[0],chunkStartPos[2]);
    biometype newbio = DESERT;
    if(biomeInfo.find(biomeKey) !=biomeInfo.end()){
        newbio = biomeInfo.at(biomeKey);
    }
    chunk->setChunkBiome(newbio);
    //PositiveX
    neighborChunk = chunkManager.getChunkByStartPos(chunkStartPos[0]+16,chunkStartPos[1],chunkStartPos[2]);
    if(neighborChunk!=nullptr){
        if(neighborChunk->getChunkBiome()!=newbio){
            posXBiome = true;
        }
    }
    //NegativeX
    neighborChunk = chunkManager.getChunkByStartPos(chunkStartPos[0]-16,chunkStartPos[1],chunkStartPos[2]);
    if(neighborChunk!=nullptr){
        if(neighborChunk->getChunkBiome()!=newbio){
            negXBiome = true;
        }
    }
    //PositiveZ
    neighborChunk = chunkManager.getChunkByStartPos(chunkStartPos[0],chunkStartPos[1],chunkStartPos[2]+16);
    if(neighborChunk!=nullptr){
        if(neighborChunk->getChunkBiome()!=newbio){
            posZBiome = true;
        }
    }
    //NegativeZ
    neighborChunk = chunkManager.getChunkByStartPos(chunkStartPos[0],chunkStartPos[1],chunkStartPos[2]-16);
    if(neighborChunk!=nullptr){
        if(neighborChunk->getChunkBiome()!=newbio){
            negZBiome = true;
        }
    }
    chunk->createChunk(blockInfo,startPos,posXBiome,negXBiome,posZBiome,negZBiome);
    chunk->setStartPos(chunkStartPos);
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
    prog_lambert.bindTexture1();
    prog_lambert.setEyePosition(Tester.eye);

    //YuxinMM03 dynamically change sky color, light color and light direction based on time
    prog_lambert.setLightDir(lightDir);
    //std::cout<<"light direction is: "<<glm::to_string(lightDir)<<std::endl;
    prog_lambert.setLightCol(lightCol);
    glClearColor(skyColor[0],skyColor[1], skyColor[2], skyColor[3]);


    GLRenderWorld();
    prog_flat.setModelMatrix(glm::mat4(1.0));
    prog_flat.setViewProjMatrix(glm::mat4(1.0));
    prog_flat.bindTexture31();
    glDisable(GL_DEPTH_TEST);
    prog_flat.draw(geom_Inventory);
    prog_flat.draw(geom_Center);
    glEnable(GL_DEPTH_TEST);
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
        flag_amount_speed = 25.0 / 60.0;
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
        walkBegin();
    } else if (e->key() == Qt::Key_S) {
//        Tester.CheckTranslateAlongLook(-amount);
//        NewChunk();
        flag_moving_backward = 1;
        walkBegin();
    } else if (e->key() == Qt::Key_D) {
        //gl_camera.TranslateAlongRight(amount);
        //Tester.CheckTranslateAlongRight(amount);
        //NewChunk();
        flag_moving_right = 1;
        walkBegin();
    } else if (e->key() == Qt::Key_A) {
//        gl_camera.TranslateAlongRight(-amount);
//        Tester.CheckTranslateAlongRight(-amount);
//        NewChunk();
        flag_moving_left = 1;
        walkBegin();
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
    }else if(e->key() == Qt::Key_C)
    {
        //Toggle day and night cycle switch
        enableDayNightCycle = !enableDayNightCycle;
    }
    //tst end
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
//Lostink insert code
void MyGL::walkBegin(){
     int yeye = Tester.eye[1]>0?int(Tester.eye[1]):int(Tester.eye[1]-1);
    if ((flag_walking == 0) && (WalkingStep == nullptr))
    {
        printf("In!\n");
        WalkingStep = audio.playFootstep(PLAIN,yeye);
        flag_walking = 1;
    }
    else
        if ((WalkingStep!=nullptr)&&(WalkingStep->isFinished()))
        {
            audio.playFootstep(PLAIN,yeye);
        }
}
void MyGL::walkEnd()
{
    if ((WalkingStep!=nullptr) && (flag_walking == 1) && (!WalkingStep->isFinished()))
    {
        audio.stopPlay(WalkingStep);
        WalkingStep = nullptr;
        flag_walking = 0;
    }
}
void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift)
    {
        flag_amount_speed = 5.0 / 60.0;
    }else if (e->key() == Qt::Key_W){
        flag_moving_forward = 0;
//        walkEnd();
    }else if (e->key() == Qt::Key_S) {
        flag_moving_backward = 0;
//        walkEnd();
    } else if (e->key() == Qt::Key_D) {
        flag_moving_right = 0;
//        walkEnd();
    } else if (e->key() == Qt::Key_A) {
        flag_moving_left = 0;
//        walkEnd();
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
void MyGL::wheelEvent(QWheelEvent *e)
{
    if (e->delta() >0){
        //printf("Fuck!\n");
        Tester.holding_type_change(-1);
    }
    else{
        //printf("EEE\n");
        Tester.holding_type_change(1);
    }
}
void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    //printf("Moving!\n");
    if (ShowMouse == false) return;
    double speed = 40;
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

void MyGL::addBlock(int x, int y, int z, blocktype bType){
    chunkManager.addBlockAt(x, y, z,bType);
}
void MyGL::timerUpdate()
{
    timeCount++;
    //pass this value to fragement shader to make water and lava shader moving
    if(timeCount==2048){
        timeCount = 0;
    }
    if(timeCount%4==0){
        prog_lambert.setTimeCount(timeCount/4);
        update();
    }


    //**********************Yuxin MM03 Day and Night Cycle************************//
    //Press keyboard C to toggle day and night cycle switch
    if(enableDayNightCycle){
        if(timeCount%32==0){
            //Change the sky color to simulate day and night cycle//
            double x = 0.0;
            x = timeCount*1.0/320;
            if((x>=PI/2 && x<=2*PI/3) || (x>=4*PI/3 && x<=3*PI/2)){
                //Sunrise and Sunset, red will dominant a little bit
                skyColor[0] = 0.5;
            }else{
               skyColor[0] = 0.2*sin(x-PI/2);
            }
            skyColor[1] = 0.5*sin(x-PI/2)+0.24;
            skyColor[2] = 0.42*sin(x-PI/2)+0.58;

            //Change the light direction to simulate sun cycle//
            double lightDirX = 0.0f;
            double lightDirY = 0.0f;
            if(x>=PI/2 && x<=3*PI/2){
                lightDirX = -sin(x);
                lightDirY = -cos(x);
            }else{
                lightDirX = -sin(x+PI);
                lightDirY = -cos(x+PI);
            }
            lightDir[0] = lightDirX;
            lightDir[1] = lightDirY;

            //Change the light color to simulate day and night cycle//
            if(x>=PI/2 && x<=3*PI/2){

                if(x<=2*PI/3 || x>=4*PI/3){
                    //Sunrise and Sunset, more red
                    lightCol[0] = 0.7;
                }else{
                    lightCol[0] = 0.3*sin(x-PI/2)+0.6;
                }
                lightCol[1] = 0.3*sin(x-PI/2)+0.6;
                lightCol[2] = 0.3*sin(x-PI/2)+0.6;
            }else{
                //Night constant dim moon light
                lightCol[0] = 0.3;
                lightCol[1] = 0.3;
                lightCol[2] = 0.3;
            }
        }
    }else{
        skyColor = glm::vec4(0.37f, 0.74f, 1.0f, 1);
        lightDir = glm::vec4(1,1,1,0);
        lightCol = glm::vec4(1,1,1,0);
    }
    //************************Yuxin MM03 Day and Night Cycle************************//


    Tester.Falling();
    update();
    Moving();
}
void MyGL::NewChunk(){
    vector<tuple<int,int,int>>* NC= Tester.GetNewBlockVec();
    int total = NC->size();
    if (total == 0) return;
    for (int i=0;i<total;++i)
    createNewChunk(terrain.mapWorld,(*NC)[i],terrain.mapBiome);
    (*NC).clear();
}
