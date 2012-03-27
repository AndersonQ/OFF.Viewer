#include "opengl.h"

OpenGL::OpenGL(QWidget *parent) :
    QGLWidget(parent)
{
    wireframe = true;
}

void OpenGL::initializeGL(){
    glEnable(GL_DEPTH_TEST);

    offr = new OFFReader((char *)"/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/cube.off");
    GetFaces(offr);

    ModelView.setToIdentity();
    MatrixProjection.setToIdentity();

    MatrixProjection.ortho(-2, 2, -2, 2, -4, 4);
    MatrixProjection.lookAt(camera.eye,camera.at,camera.up);

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

    /* Set projections */
    switch(camera.projection)
    {
        case ORTHO:
            MatrixProjection.ortho(camera.left, camera.right, camera.bottom, camera.top, camera.nearplane, camera.farplane);
            break;
        case PERSPECTIVE:
            MatrixProjection.perspective(camera.fovy, (camera.a/camera.b), camera.nearplane, camera.farplane);
            break;
        case FRUSTUM:
            MatrixProjection.frustum(camera.left, camera.right, camera.bottom, camera.top, camera.nearplane, camera.farplane);
            break;
    }

    ModelView.setToIdentity();
    ModelView.lookAt(camera.eye, camera.at, camera.up);
    ModelView.rotate(rotatey,0,1,0);

    m_shaderProgram->setUniformValue("Projection", MatrixProjection);
    m_shaderProgram->setUniformValue("ModelView", ModelView);
    //m_shaderProgram->setUniformValue("NormalMatrix", Normal);

    /* VBO of vertex */
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition", GL_FLOAT, 0, 3, 0);

    /* VBO of colours */
    m_vboColours->bind();
    m_shaderProgram->enableAttributeArray("vColour");
    m_shaderProgram->setAttributeBuffer("vColour", GL_FLOAT, 0, 3, 0);

    if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
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

/* Slots */
void OpenGL::SetProjection(int p){
    camera.projection = p;
    updateGL();
}

void OpenGL::SetLeft(double n){
    camera.left = n;
    updateGL();
}

void OpenGL::SetRight(double n){
    camera.right = n;
    updateGL();
}

void OpenGL::Setbottom(double n){
    camera.bottom = n;
    updateGL();
}

void OpenGL::SetTop(double n){
    camera.top = n;
    updateGL();
}

void OpenGL::SetNearplane(double n){
    camera.nearplane = n;
    updateGL();
}

void OpenGL::SetFarplane(double n){
    camera.farplane = n;
    updateGL();
}

void OpenGL::SetEyex(double n){
    camera.eye.setX(n);
    updateGL();
}

void OpenGL::SetEyey(double n){
    camera.eye.setY(n);
    updateGL();
}

void OpenGL::SetEyez(double n){
    camera.eye.setZ(n);
    updateGL();
}

void OpenGL::SetLookatx(double n){
    camera.at.setX(n);
    updateGL();
}

void OpenGL::SetLookaty(double n){
    camera.at.setY(n);
    updateGL();
}

void OpenGL::SetLookatz(double n){
    camera.at.setZ(n);
    updateGL();
}

void OpenGL::SetUpx(double n){
    camera.up.setX(n);
    updateGL();
}

void OpenGL::SetUpy(double n){
    camera.up.setY(n);
    updateGL();
}

void OpenGL::SetUpz(double n){
    camera.up.setZ(n);
    updateGL();
}

void OpenGL::Seta(double n){
    camera.a = n;
    updateGL();
}

void OpenGL::Setb(double n){
    camera.b = n;
    updateGL();
}

void OpenGL::SetAnglefovy(double n){
    camera.fovy = n;
    updateGL();
}

void OpenGL::SetWireframe(bool b){
    wireframe = b;
    updateGL();
}

void OpenGL::SetCullface(bool c)
{
    if(c)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    updateGL();
}

void OpenGL::SetOnecolour(bool b){
    updateGL();
}

void OpenGL::SetColourgray(bool b){
    updateGL();
}
