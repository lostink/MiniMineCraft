#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <glwidget277.h>
#include <la.h>
#include <glm/glm.hpp>

#include "drawable.h"
#include <soillib/src/SOIL.h>


class ShaderProgram
{
public:
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint prog;       // A handle for the linked shader program stored in this class

    //Yuxin MM02
    GLuint textureHandler; //A handle for the texture map loaded for lambert shader
    GLuint normalmapHandler; //A handle for the nornal map loaded for lambert shader

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader

    //Yuxin MM02
    int unifSampler; //A handle for the "uniform" textureSampler representing the texture map in fragment shader
    int attrUv; //A handle for the "in" vec2 representing uv coordinates in the vertex shader
    int unifNormal; //A handle for the "uniform" normalSampler representing the normal map in fragment shader
    int unifTime; //A handle for the "uniform" int u_Time representing a timer which can alter color of fragments
    int attrBlockType; //A handle for the "in" float representing the block uv animated type
    int attrShiness; //A handle for the "in" float representing the block shiness
    int unifEye; //A handle for the "uniform" vec4 representing the eye position in the world


public:
    ShaderProgram(GLWidget277* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    // Tells our OpenGL context to use this shader to draw things
    void useMe();
    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);
    // Draw the given object to our screen using this ShaderProgram's shaders
    void draw(Drawable &d);
    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    QString qTextFileRead(const char*);

    //Yuxin MM02
    void bindTexture0();
    void setUpTexture();
    void bindNormalMap0();
    void setUpNormalMap();
    void setTimeCount(int timeCount);
    void setEyePosition(glm::vec3 eyePos);

private:
    GLWidget277* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};


#endif // SHADERPROGRAM_H
