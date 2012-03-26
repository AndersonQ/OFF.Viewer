#ifndef OPENGL_H
#define OPENGL_H

#include <QGLWidget>

#include <QGLShader>
#include <QGLBuffer>
#include <QVector3D>
#include <QTimer>

#include "camera.h"
#include "offreader.h"

class OpenGL : public QGLWidget
{
    Q_OBJECT
public:
    explicit OpenGL(QWidget *parent = 0);

protected:
    /* Variables */
    OFFReader *offr;

    QGLShader *m_vertexShader;
    QGLShader *m_fragmentShader;
    QGLShaderProgram *m_shaderProgram;

    QGLBuffer *m_vboVertices;
    QGLBuffer *m_vboNormals;
    QGLBuffer *m_vboColours;
    QGLBuffer *m_vboIndices;

    QVector3D *faces;
    QVector3D *colours;
    int num_vertices, num_faces, num_edge;

    QMatrix4x4 ModelView, Projection;
    QMatrix3x3 Normal;

    Camera camera;

    QTimer *timer;

    float rotatey;

    /* Functions */
    void initializeGL();
    void paintGL();

    void InitializeVBOs();

    void initializeMode1();

    void GetFaces(OFFReader *offr);

signals:

public slots:
    void Spin();

};

#endif // OPENGL_H
