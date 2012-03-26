#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>

#define ORTHO 0
#define PERSPECTIVE 1
#define FRUSTUM 2

class Camera
{
public:
    /* Functions */
    Camera();

    /* variables */
    QVector3D eye;
    QVector3D up;
    QVector3D at;

    int projection, fovy;
    float right, left, top, bottom, nearplane, farplane, a, b;
};

#endif // CAMERA_H
