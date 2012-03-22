#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>

class Camera
{
public:
    Camera();

    QVector3D eye;
    QVector3D up;
    QVector3D at;
};

#endif // CAMERA_H
