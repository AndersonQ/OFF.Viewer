#include "camera.h"

Camera::Camera()
{
    eye = QVector3D(0.0, 0.0, 2);
    up  = QVector3D(0.0, 1.0, 0.0);
    at  = QVector3D(0.0, 0.0, 0.0);

    projection = 0;
    fovy = 45.0;
    right = -2.0;
    left = 2.0;
    top = 2.0;
    bottom = -2.0;
    nearplane = 4.0;
    farplane = 10.0;
    a = 16.0;
    b = 9.0;
}
