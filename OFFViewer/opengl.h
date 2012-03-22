#ifndef OPENGL_H
#define OPENGL_H

#include <QGLWidget>

#include <QGLShader>
#include <QGLBuffer>
#include <QVector3D>

class OpenGL : public QGLWidget
{
    Q_OBJECT
public:
    explicit OpenGL(QObject *parent = 0);

protected:
    /* Variables */
    QGLShader *m_vertexShader;
    QGLShader *m_fragmentShader;
    QGLShaderProgram *m_shaderProgram;

    QGLBuffer *m_vboVertices;
    QGLBuffer *m_vboNormals;
    QGLBuffer *m_vboColors;
    QGLBuffer *m_vboIndices;

    QVector4D *verticesphere;
    QVector3D *normalsphere;
    unsigned int num_vertices, num_faces, num_edge;

    QMatrix3x3 ModelView, Projection, Normal;


    /* Functions */
    void initializeGL();

signals:

public slots:

};

#endif // OPENGL_H
