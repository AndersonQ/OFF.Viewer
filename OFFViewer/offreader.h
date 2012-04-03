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

#ifndef OFFREADER_H
#define OFFREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> /* To use setlocale */

#include <QVector3D>
#include <QMessageBox>

class OFFReader
{
public:
    /* Functions */
    OFFReader(char *name);
    ~OFFReader();

    void readoff();

    /* Variables */
    int num_vertices, num_faces, num_edge;
    int **faces;
    float **vertices;


protected:
    FILE *off;
    char *file_name;
    char l1, l2, l3;
};

#endif // OFFREADER_H
