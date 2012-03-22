#include "offreader.h"

#define ALOCATED(ptr) if(!ptr) { printf("Memory is Full!\n"); exit(1); }

OFFReader::OFFReader(char *name)
{
    //Open file
    file_name = name;
    off = fopen(name, "r");


    if(!off)
        printf("ERROR: Can not open file %s\n", name); //Error if can't open file
    else{
        fscanf(off,"%s", line1);
        if(strcmp(line1, "OFF") != 0)
            printf("File %s is not a OFF file!", name);
    }

    readoff();
}


void OFFReader::readoff(){
    int read;

    /* Read number of vertex, face and edge */
    fscanf(off, "%d,%d,%d", &num_vertices, &num_faces, &num_edge);

    /* Alocate the array of vertices */
    vertices = (float **) malloc(sizeof(float*)*num_vertices);
    ALOCATED(vertices)

    /* Alocate memory to vectors */
    for(int i = 0; i < num_vertices; i++){
        vertices[i] = (float *) malloc(sizeof(float)*3);
        ALOCATED(vertices[i])
    }

    /* Alocate the array of faces */
    faces = (int **) malloc(sizeof(int *)*3);
    ALOCATED(faces)

    /* Alocate memory to faces */
    for(int i = 0; i < num_faces; i++)
    {
        faces[i] = (int *) malloc(sizeof(int)*3);
        ALOCATED(faces[i])
    }

    /* Set Locale to use dot like a decimal separator */
    setlocale(LC_ALL, "C.UTF-8");

    /* Reading the vertices */
    for(int i = 0; i < num_vertices; i++){
        read = fscanf(off, "%f,%f,%f", &(vertices[i][0]), &(vertices[i][1]), &(vertices[i][2]));

        /* Degub */
        printf("Vertex[%d] = (%f,%f,%f) read = %d\n", i, vertices[i][0], vertices[i][1], vertices[i][2], read);

        if (read != 3){
            printf("ERROR: Reading file \"%s\", format is invalid!", file_name);
            exit(1);
        }
    }

    /* Reading the faces */
    for(int i = 0; i < num_faces; i++){
        read = fscanf(off, "%d,%d,%d", &(faces[i][0]), &(faces[i][1]), &(faces[i][2]));

        /* Degub */
        printf("Face[%d] = (%d,%d,%d) read = %d\n", i, faces[i][0], faces[i][1], faces[i][2], read);

        if (read != 3){
            printf("ERROR: Reading file \"%s\", format is invalid!", file_name);
            exit(1);
        }
    }

    fclose(off);
}
