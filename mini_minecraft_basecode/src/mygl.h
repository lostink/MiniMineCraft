#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cube.h>
#include "camera.h"
#include <scene/scene.h>
#include <scene/chunk.h>
#include <scene/chunkmanager.h>
#include <scene/inventory.h>
#include <proceduralterrain.h>
#include <audiomanager.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
//Code from lostink for testing character
#include <character.h>
#include <tuple>
#include <map>
#include <scene/screencenter.h>
using std::tuple;
using std::map;
//tester code end

class MyGL
    : public GLWidget277
{
private:
    Cube geom_cube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    screenCenter geom_Center;
    Inventory geom_Inventory;
    ChunkManager chunkManager;
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)
    int timeCount; //For updating moving texture
    //Yuxin MM03
    glm::vec4 skyColor;
    glm::vec4 lightDir;
    glm::vec4 lightCol;
    bool enableDayNightCycle;

    ProceduralTerrain terrain;
    AudioManager audio;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;
    Scene scene;

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;
    //Code from lostink for testing character
    character Tester;
    bool ShowMouse;
//tester code end
public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();

    //==============Added By Yuxin=============//
    void GLRenderWorld();
    void createNewChunk(std::map<std::tuple<int, int, int>, blocktype> &blockInfo, std::tuple<int, int, int> startPos
                        ,std::map<std::pair<int,int>, biometype> &biomeInfo);
    void updateChunkVBO();
    void updateChunkVisibility();
    void deleteBlock(int x, int y, int z);
    void addBlock(int x, int y, int z, blocktype bType);
    void NewChunk();
protected:
    void keyPressEvent(QKeyEvent *e);
    //Lostink insert code
    QSound* WalkingStep;
    bool flag_moving_forward;
    bool flag_moving_backward;
    bool flag_moving_right;
    bool flag_moving_left;
    bool flag_moving_up;      //Test mode only
    bool flag_moving_down;    //Test mode only
    bool flag_rotate_right;
    bool flag_rotate_left;
    bool flag_rotate_up;
    bool flag_rotate_down;
    int flag_walking;
    float flag_amount_speed;
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void walkBegin();
    void walkEnd();
    void Moving();
    //Insert End.
private slots:
    /// Slot that gets called ~60 times per second
    virtual void timerUpdate();
};


#endif // MYGL_H
