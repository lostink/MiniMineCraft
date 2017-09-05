#ifndef INVENTORY_H
#define INVENTORY_H
#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Inventory: public Drawable
{
public:
    Inventory(GLWidget277* context): Drawable(context){}
    virtual void create();
    QPoint Center;
    void setCenter(QPoint Now);
    GLenum drawMode();
};

#endif // INVENTORY_H
