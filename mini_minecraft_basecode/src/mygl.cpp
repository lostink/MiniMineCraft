#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cube(this),geom_Center(this),
      prog_lambert(this), prog_flat(this),
      gl_camera()
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    //Lostink Test code
    tuple<int,int,int> a(32,20,32),b(32,22,34);
    test[a] = 1;
    test[b] = 1;
    Tester.SetMainCamera(&gl_camera);
    Tester.SetMesh(&test);
    geom_Center.setCenter(QPoint(width() / 2, height() / 2));
    ShowMouse = true;
    setMouseTracking(true);
    QCursor c = cursor();
    c.setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
    setCursor(c);
    //Test end.
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    geom_cube.destroy();
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

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

    scene.CreateTestScene();
}

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

    GLDrawScene();
    //Lostink insert code here to draw screen center
    prog_flat.setModelMatrix(glm::mat4(1.0));
    prog_flat.setViewProjMatrix(glm::mat4(1.0));
    prog_flat.draw(geom_Center);
    //Insert end.
}

void MyGL::GLDrawScene()
{
    for(int x = 0; x < scene.objects.size(); x++)
    {
        QList<QList<bool>> Xs = scene.objects[x];
        for(int y = 0; y < Xs.size(); y++)
        {
            QList<bool> Ys = Xs[x];
            for(int z = 0; z < Ys.size(); z++)
            {
                if(Ys[z])
                {
                    prog_lambert.setModelMatrix(glm::translate(glm::mat4(), glm::vec3(y, x, z)));
                    prog_lambert.draw(geom_cube);
                }
            }
        }
    }
}
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
//        gl_camera.RotateAboutUp(-amount);
        Tester.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
//        gl_camera.RotateAboutUp(amount);
        Tester.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
//        gl_camera.RotateAboutRight(-amount);
        Tester.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
//        gl_camera.RotateAboutRight(amount);
        Tester.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        //gl_camera.TranslateAlongLook(amount);
        Tester.CheckTranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
//        gl_camera.TranslateAlongLook(-amount);
        Tester.CheckTranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        //gl_camera.TranslateAlongRight(amount);
        Tester.CheckTranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
//        gl_camera.TranslateAlongRight(-amount);
        Tester.CheckTranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
//        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
//        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
//        gl_camera = Camera(this->width(), this->height());
    }
    //Test code:
        else if (e->key() == Qt::Key_Space){
        Tester.Jump();
    }   else if (e->key() == Qt::Key_F3)
    {
        ShowMouse^=1;
    }
    //tst end
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
//Lostink insert code
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
//Insert End.
void MyGL::timerUpdate()
{
    //Test code from Lostink
    //Testing physical system
        //printf("=====================\n");
        //printf("%lf %lf %lf\n",gl_camera.eye[0],gl_camera.eye[1],gl_camera.eye[2]);
        Tester.Falling();
        update();
    //Test code end
}
//
