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

    int projection;
    double right, left, top, bottom, nearplane, farplane, a, b, fovy;
};

#endif // CAMERA_H
