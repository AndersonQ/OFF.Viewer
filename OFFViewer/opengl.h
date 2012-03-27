#ifndef OPENGL_H
#define OPENGL_H

#include <QGLWidget>

#include <QGLShader>
#include <QGLBuffer>
#include <QVector3D>
#include <QTimer>
#include <QMouseEvent>

#include "camera.h"
#include "offreader.h"
#include "trackball.h"

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

    QMatrix4x4 ModelView, MatrixProjection, MatrixRotation;

    Camera camera;

    QTimer *timer;

    bool wireframe;

    float zoom;

    TrackBall trackball;

    /* Functions */
    void initializeGL();
    void paintGL();

    void InitializeVBOs();

    void initializeMode1();

    void GetFaces(OFFReader *offr);

    /* Trackball */
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent (QMouseEvent *event);
    void mouseReleaseEvent (QMouseEvent *event);
    void wheelEvent (QWheelEvent *event);
    QPointF toSpherepos(const QPointF& p);

signals:

public slots:
    void Spin();
    void SetProjection(int p);
    void SetLeft(double n);
    void SetRight(double n);
    void Setbottom(double n);
    void SetTop(double n);
    void SetNearplane(double n);
    void SetFarplane(double n);
    void Seta(double n);
    void Setb(double n);
    void SetAnglefovy(double n);
    void SetEyex(double n);
    void SetEyey(double n);
    void SetEyez(double n);
    void SetLookatx(double n);
    void SetLookaty(double n);
    void SetLookatz(double n);
    void SetUpx(double n);
    void SetUpy(double n);
    void SetUpz(double n);
    void SetWireframe(bool b);
    void SetCullface(bool c);
    void SetOnecolour(bool b);
    void SetColourgray(bool b);

};

#endif // OPENGL_H
