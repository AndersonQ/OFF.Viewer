#include "offreader.h"

#define ALOCATED(ptr) if(!ptr) { printf("Can not allocate memory, Memory is Full!\n"); exit(1); }

OFFReader::OFFReader(char *name)
{
    //Open file
    file_name = name;
    off = fopen(file_name, "r");

    if(off == 0x0)
        printf("ERROR: Can not open file %s\n", name); //Error if can't open file
    else{
        fscanf(off,"%c%c%c",&l1,&l2,&l3);
        if(l1 != 'O' && l2 != 'F' && l3 != 'F'){//strcmp(line1, "OFF") == 0)
            printf("ERROR: File \"%s\" is not a OFF file!", name);
            fclose(off);
            fflush(stdout);
            exit(0);
        }
    }

    readoff();
}

OFFReader::~OFFReader(){

    /* Frees the memory space alocatede to vertives */
    if(vertices){
        for(int i = 0; i < num_vertices; i++)
            free(vertices[i]);
        free(vertices);
    }

    /* Frees the memory space alocatede to faces */
    if(vertices){
        for(int i = 0; i < num_faces; i++)
            free(faces[i]);
        free(faces);
    }
}

void OFFReader::readoff(){
    int read, n, line = 2;

    /* Read number of vertex, face and edge */
    read = fscanf(off, "%d %d %d", &num_vertices, &num_faces, &num_edge);

    /* Degub */
    printf("%d num_vertices = %d, num_faces = %d, num_edge = %d, read = %d\n", line ++, num_vertices, num_faces, num_edge, read);
    fflush(stdout);

    /* Alocate the array of vertices */
    vertices = (float **) malloc(sizeof(float*)*num_vertices);
    ALOCATED(vertices)

    /* Alocate memory to vectors */
    for(int i = 0; i < num_vertices; i++){
        vertices[i] = (float *) malloc(sizeof(float)*3);
        ALOCATED(vertices[i])
    }

    /* Alocate the array of faces */
    faces = (int **) malloc(sizeof(int *)*num_faces);
    ALOCATED(faces)

    /* Alocate memory to faces */
    for(int i = 0; i < num_faces; i++)
    {
        faces[i] = (int *) malloc(sizeof(int)*3);
        ALOCATED(faces[i])
    }

    /* Set Locale to use dot like a decimal separator */
    setlocale(LC_ALL, "en_US.utf8");

    /* Reading the vertices */
    for(int i = 0; i < num_vertices; i++){
        read = fscanf(off, "%f %f %f\n", &(vertices[i][0]), &(vertices[i][1]), &(vertices[i][2]));

        /* Degub */
        printf("%d Vertex[%d] = (%f, %f, %f) read = %d\n",line++ , i, vertices[i][0], vertices[i][1], vertices[i][2], read);
        fflush(stdout);

        if (read != 3){
            printf("ERROR: Reading file \"%s\", format is invalid!\n", file_name);
            fflush(stdout);
            exit(1);
        }
    }

    /* Reading the faces */
    for(int i = 0; i < num_faces; i++){
        read = fscanf(off, "%d %d %d %d", &n, &(faces[i][0]), &(faces[i][1]), &(faces[i][2]));

        /* Degub */
        printf("%d Face[%d] = (%d, %d, %d, %d) read = %d\n",line++, i, n, faces[i][0], faces[i][1], faces[i][2], read);
        fflush(stdout);

        if (read != 4){
            printf("ERROR: Reading file \"%s\", format is invalid! Read %d, but expected 4\n", file_name, read);
            fflush(stdout);
            exit(1);
        }
        if (n != 3){
            printf("ERROR: Reading file \"%s\", format is invalid! Only triangles are acepted!\n", file_name);
            fflush(stdout);
            exit(1);
        }
    }

    fclose(off);
}
