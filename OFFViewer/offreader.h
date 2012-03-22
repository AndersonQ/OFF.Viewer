#ifndef OFFREADER_H
#define OFFREADER_H

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> /* To use setlocale */

class OFFReader
{
public:
    OFFReader(char *name);
    ~OFFReader();

    void readoff();

protected:
    FILE *off;
    char *file_name;
    int num_vertices, num_faces, num_edge, **faces;
    float **vertices;
    char *line1;
};

#endif // OFFREADER_H
