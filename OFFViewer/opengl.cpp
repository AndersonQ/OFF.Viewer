#include "opengl.h"

OpenGL::OpenGL(QObject *parent) :
    QGLWidget(parent)
{
}

void OpenGL::initializeGL(){
    glEnable(GL_DEPTH_TEST);

    ModelView.setToIdentity();

}
