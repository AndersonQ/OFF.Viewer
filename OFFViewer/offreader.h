#ifndef OFFREADER_H
#define OFFREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> /* To use setlocale */

#include <QVector3D>

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
