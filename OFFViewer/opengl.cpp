#include "opengl.h"

OpenGL::OpenGL(QWidget *parent) :
    QGLWidget(parent)
{
}

void OpenGL::initializeGL(){
    glEnable(GL_DEPTH_TEST);

    offr = new OFFReader((char *)"/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/cube.off");
    GetFaces(offr);

    ModelView.setToIdentity();
    Projection.setToIdentity();

    Projection.ortho(-2, 2, -2, 2, -4, 4);
    Projection.lookAt(camera.eye,camera.at,camera.up);

    /* Initialize shaders */
    m_vertexShader = new QGLShader(QGLShader::Vertex);
    m_fragmentShader = new QGLShader(QGLShader::Fragment);

    if(!m_vertexShader->compileSourceFile(":/vshader.glsl"))
        qWarning() << m_vertexShader->log();

    if(!m_fragmentShader->compileSourceFile(":/fshader.glsl"))
        qWarning() << m_fragmentShader->log();

    m_shaderProgram = new QGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);

    if(!m_shaderProgram->link())
        qWarning() << m_shaderProgram->log() << endl;
    else
        m_shaderProgram->bind();

    InitializeVBOs();
    glClearColor(0.0, 1.0, 0.0, 1.0);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Spin()));
    timer->start(1);
}

void OpenGL::InitializeVBOs(){

    /* Initialize the points that will be draw */
    GetFaces(offr);

    colours = new QVector3D[offr->num_faces * 3];

    /* Initialize vertex's colours */
    for(int i = 0; i < offr->num_faces*3; i++)
        colours[i] = QVector3D(0.8, 0.8, 0.8);
        /*colours[i] = QVector3D(rand()/(float)(RAND_MAX),
                               rand()/(float)(RAND_MAX),
                               rand()/(float)(RAND_MAX));*/

    /* Creat VBos to vertices */
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(faces, offr->num_faces*3*sizeof(QVector3D));
    delete[] faces;
    faces = NULL;

    m_vboColours = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboColours->create();
    m_vboColours->bind();
    m_vboColours->setUsagePattern(QGLBuffer::DynamicDraw);
    m_vboColours->allocate(colours, offr->num_faces * 3 * sizeof(QVector3D));
    delete[] colours;
    colours = NULL;

}

void OpenGL::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    /* put more projections */
    Projection.ortho(-2, 2, -2, 2, -4, 4);

    ModelView.setToIdentity();
    ModelView.lookAt(camera.eye, camera.at, camera.up);
    ModelView.rotate(rotatey,0,1,0);
    Normal.setToIdentity();

    m_shaderProgram->setUniformValue("Projection", Projection);
    m_shaderProgram->setUniformValue("ModelView", ModelView);
    m_shaderProgram->setUniformValue("NormalMatrix", Normal);

    /* VBO of vertex */
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition", GL_FLOAT, 0, 3, 0);

    /* VBO of colours */
    m_vboColours->bind();
    m_shaderProgram->enableAttributeArray("vColour");
    m_shaderProgram->setAttributeBuffer("vColour", GL_FLOAT, 0, 3, 0);

    //if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //else
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLES, 0, offr->num_faces * 3);
    printf("DrawArrays\n");
    fflush(stdout);

    m_vboVertices->release();
    m_vboColours->release();
}

void OpenGL::initializeMode1(){
}

void OpenGL::GetFaces(OFFReader *offr){

    /* Building a vertices array without an indices array */
    faces = new QVector3D[offr->num_faces * 3]; //The faces are triangles

    for (int i = 0; i < offr->num_faces; i++){

        faces[i*3].setX( offr->vertices[ offr->faces[i][0] ][0]);
        faces[i*3].setY( offr->vertices[ offr->faces[i][0] ][1]);
        faces[i*3].setZ( offr->vertices[ offr->faces[i][0] ][2]);

        faces[i*3+1].setX( offr->vertices[ offr->faces[i][1] ][0]);
        faces[i*3+1].setY( offr->vertices[ offr->faces[i][1] ][1]);
        faces[i*3+1].setZ( offr->vertices[ offr->faces[i][1] ][2]);

        faces[i*3+2].setX( offr->vertices[ offr->faces[i][2] ][0]);
        faces[i*3+2].setY( offr->vertices[ offr->faces[i][2] ][1]);
        faces[i*3+2].setZ( offr->vertices[ offr->faces[i][2] ][2]);

        /*faces[i*3+2] = QVector3D( offr->vertices[ offr->faces[i][2] ] [0],
                                offr->vertices[ offr->faces[i][2] ] [1],
                                offr->vertices[ offr->faces[i][2] ] [2] );*/
    }

}

void OpenGL::Spin(){
    rotatey = (rotatey<360 ? rotatey+0.02 : 360-rotatey);
    updateGL();
}
