/*
 * This file is part of OFFViewer.
 *
 * OFFViewer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OFFViewer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OFFViewer.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include "light.h"
#include "material.h"

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
    QGLBuffer *m_vboNormal;
    QGLBuffer *m_vboColours;
    QGLBuffer *m_vboIndices;
    QGLBuffer *m_vboTexCoords;

    QVector3D *faces;
    QVector3D *colours;

    QVector2D *texcoords;
    QVector3D *normal, *tangents, *bitangents;
    QVector4D *vertices;
    unsigned int *indices;
    GLint num_vertices, num_faces, num_edge;

    QMatrix4x4 ModelView, MatrixProjection, MatrixRotation;
    QMatrix3x3 MatrixNormal;

    Camera camera;
    Light light;
    Material material;

    QTimer *timer;

    QImage *texColor, *texNormal;
    int SelectedImage, SelectedCoord;

    bool wireframe;
    bool OneColour;
    bool Colourgold;

    float zoom;

    int Shader, TexIndice;
    GLint BgRed, BgGreen, BgBlue;

    TrackBall trackball;

    /* Functions */
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void initFlatShading();
    void initGouraudShading();
    void initPhong();
    void initPhongHalf();
    void initCartoon();
    void initSimpleTexMapping();
    void initCubeMapping();

    void UseFlatShading();
    void UseGouraud();
    void UsePhong();
    void UsePhongHalf();
    void UseCartoon();
    void UseSimpleTexMapping();
    void UseCubeMapping();

    void CreateVertexIndices();

    void ChooseTexture();
    void ChooseCoordinates();
    void GenTexCoordsCylinder();
    void GenTexCoordsSphere();
    void GenTexCoordsTriangles();

    void ChangeShader(int s);

    void LoadShaders(std::string const &s1, std::string const &s2);
    void CalculateNormal();

    void InitializeVBOs();

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
    void SetBottom(double n);
    void SetTop(double n);
    void SetNearplane(double n);
    void SetFarpalne(double n);
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
    void SetColourgold(bool b);
    void LoadOFF(void);
    void SetBgRed(int r);
    void SetBgGreen(int g);
    void SetBgBlue(int b);
    void SetShader(int s);

};

#endif // OPENGL_H
