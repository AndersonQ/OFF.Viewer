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
