#ifndef SCREENCENTER_H
#define SCREENCENTER_H
#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class screenCenter: public Drawable
{
public:
    screenCenter(GLWidget277* context): Drawable(context){}
    virtual void create();
    QPoint Center;
    void setCenter(QPoint Now);
    GLenum drawMode();
};

#endif // SCREENCENTER_H
