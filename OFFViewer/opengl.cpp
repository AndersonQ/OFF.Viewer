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

#include "opengl.h"

OpenGL::OpenGL(QWidget *parent) :
    QGLWidget(parent)
{
    wireframe = false;
    trackball = TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);
    zoom = 100;
}

void OpenGL::initializeGL(){
    glEnable(GL_DEPTH_TEST);

    offr = new OFFReader((char *) "/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/cube.off");

    /* Initialize all Matrix like identity matrix */
    ModelView.setToIdentity();
    MatrixProjection.setToIdentity();
    MatrixRotation.setToIdentity();

    /* Default projection */
    MatrixProjection.ortho(-2, 2, -2, 2, -4, 4);
    MatrixProjection.lookAt(camera.eye,camera.at,camera.up);//QVector3D(0.0, 0.0, 2), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));

    /* Default scale */
    ModelView.scale(zoom/200);

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
        //colours[i] = QVector3D(0.8, 0.8, 0.8);
        colours[i] = QVector3D(rand()/(float)(RAND_MAX),
                               rand()/(float)(RAND_MAX),
                               rand()/(float)(RAND_MAX));

    /* Creat VBO to vertices */
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(faces, offr->num_faces*3*sizeof(QVector3D));
    delete[] faces;
    faces = NULL;

    /* Creat VBO to colours */
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

    /* Reset matrix */
    ModelView.setToIdentity();
    MatrixProjection.setToIdentity();

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
            MatrixProjection.frustum((qreal)camera.left, (qreal)camera.right, (qreal)camera.bottom, (qreal)camera.top, (qreal)camera.nearplane, (qreal)camera.farplane);
            break;
    }

    /* Zoom */
    ModelView.scale(zoom/100);

    /* Set lookat */
    ModelView.lookAt(camera.eye, camera.at, camera.up);

    /* Set rotation */
    MatrixRotation.rotate(trackball.rotation());

    /* Send matrix to shader */
    m_shaderProgram->setUniformValue("MatrixProjection", MatrixProjection);
    m_shaderProgram->setUniformValue("MatrixModelView", ModelView);
    m_shaderProgram->setUniformValue("MatrixRotation", MatrixRotation);

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
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLES, 0, offr->num_faces * 3);

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
    updateGL();
}

/* Mouse functions */

void OpenGL::mousePressEvent(QMouseEvent *event){
    if(event->buttons() & Qt::LeftButton)
    {
        trackball.push(toSpherepos(event->posF()), QQuaternion());
        event->accept();
    }
}

void OpenGL::mouseMoveEvent (QMouseEvent *event){
    if(event->buttons() & Qt::LeftButton)
    {
        trackball.move(toSpherepos(event->posF()), QQuaternion());
        updateGL();
        event->accept();
    }
    else
        trackball.release(toSpherepos(event->posF()), QQuaternion());
}

void OpenGL::mouseReleaseEvent (QMouseEvent *event){
    if(event->buttons() & Qt::LeftButton)
    {
        trackball.release(toSpherepos(event->posF()), QQuaternion());
        event->accept();
    }
}

void OpenGL::wheelEvent (QWheelEvent *event){
    if (event->delta() > 0)
        zoom += 5;
    else
        zoom -= 5;

    if (zoom >= 1000)
        zoom = 1000;
    if (zoom <= 0)
        zoom = 1;
}

QPointF OpenGL::toSpherepos(const QPointF& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0, 1.0 - 2.0 * float(p.y()) / height());
}

/* Slots */
void OpenGL::SetProjection(int p){
    camera.projection = p;
}

void OpenGL::SetLeft(double n){
    camera.left = n;
}

void OpenGL::SetRight(double n){
    camera.right = n;
}

void OpenGL::Setbottom(double n){
    camera.bottom = n;
}

void OpenGL::SetTop(double n){
    camera.top = n;
}

void OpenGL::SetNearplane(double n){
    camera.nearplane = n;
}

void OpenGL::SetFarplane(double n){
    camera.farplane = n;
}

void OpenGL::SetEyex(double n){
    camera.eye.setX(n);
}

void OpenGL::SetEyey(double n){
    camera.eye.setY(n);
}

void OpenGL::SetEyez(double n){
    camera.eye.setZ(n);
}

void OpenGL::SetLookatx(double n){
    camera.at.setX(n);
}

void OpenGL::SetLookaty(double n){
    camera.at.setY(n);
}

void OpenGL::SetLookatz(double n){
    camera.at.setZ(n);
}

void OpenGL::SetUpx(double n){
    camera.up.setX(n);
}

void OpenGL::SetUpy(double n){
    camera.up.setY(n);
}

void OpenGL::SetUpz(double n){
    camera.up.setZ(n);
}

void OpenGL::Seta(double n){
    camera.a = n;
}

void OpenGL::Setb(double n){
    camera.b = n;
}

void OpenGL::SetAnglefovy(double n){
    camera.fovy = n;
}

void OpenGL::SetWireframe(bool b){
    wireframe = b;
}

void OpenGL::SetCullface(bool c)
{
    if(c)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

void OpenGL::SetOnecolour(bool b){
}

void OpenGL::SetColourgray(bool b){
}
