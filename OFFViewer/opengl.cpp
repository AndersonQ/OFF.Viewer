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
    /* Set NULL all pointers */
    m_vertexShader = NULL;
    m_fragmentShader = NULL;
    m_shaderProgram = NULL;
    m_vboVertices = NULL;
    m_vboNormal = NULL;
    m_vboColours = NULL;
    m_vboIndices = NULL;
    vertices = NULL;
    indices = NULL;
    normal = NULL;

    /* Backgound initial colour */
    BgRed = BgGreen = BgBlue = 0;

    /* Set default shading */
    Shader = 0;

    trackball = TrackBall(0.01f, QVector3D(0, 1, 0), TrackBall::Sphere);

    /* Load OFF file *
    QString file = QFileDialog::getOpenFileName(NULL, "Load OFF file", QDir::currentPath() + QString("../"), tr(" *.off *.OFF;;All Files(*)"));
    if (file.length() != 0){
        offr = new OFFReader((char *) file.toStdString().c_str());
    }*/
}

void OpenGL::initializeGL(){
    glEnable(GL_DEPTH_TEST);

    wireframe = true;
    zoom = 100;

    offr = new OFFReader((char *) "/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/Models.OFF/sphere.off");

    /* Initialize all Matrix like identity matrix */
    ModelView.setToIdentity();
    MatrixProjection.setToIdentity();
    MatrixRotation.setToIdentity();
    MatrixNormal.setToIdentity();

    /* Default projection */
    MatrixProjection.ortho(-2, 2, -2, 2, -4, 4);
    MatrixProjection.lookAt(camera.eye,camera.at,camera.up);//QVector3D(0.0, 0.0, 2), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));

    /* Default scale */
    //ModelView.scale(zoom/100);

    /* Initialize shaders *
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
        m_shaderProgram->bind();*/

    //DEBUG
    glPointSize(10.0);

    //InitializeVBOs();
    init_FlatShading();
    glClearColor(0.0, 1.0, 0.0, 1.0);

    /* Cullface true */
    glEnable(GL_CULL_FACE);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Spin()));
    timer->start(1);
}

void OpenGL::CreateVertexIndices()
{
    /* Create Vector4D to vertices */
    if (vertices){
        delete[] vertices;
        vertices = NULL;
    }
    vertices = new QVector4D[offr->num_vertices];
    for(int i = 0; i < offr->num_vertices; i++){
        vertices[i].setX(offr->vertices[i][0]);
        vertices[i].setY(offr->vertices[i][1]);
        vertices[i].setZ(offr->vertices[i][2]);
        vertices[i].setW(1.0);
    }
    /*DEBUG
    vertices[0] = QVector4D(-1.0, -1.0, 0.0, 1.0);
    vertices[1] = QVector4D(1.0, 1.0, 0.0, 1.0);
    vertices[2] = QVector4D(0.5, 0.5, 0.0, 1.0);
    vertices[3] = QVector4D(-1.0, -1.0, 1.0, 1.0);*/

    /* Create vector to indices */
    if(indices){
        delete[] indices;
        indices = NULL;
    }
    indices = new unsigned int[offr->num_faces * 3];
    for(int i = 0; i < offr->num_faces; i++){
        indices[i*3   ] = offr->faces[i][0];
        indices[i*3 +1] = offr->faces[i][1];
        indices[i*3 +2] = offr->faces[i][2];
    }
    /*DEBUG
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;*/

}

void OpenGL::init_FlatShading(){

    QVector4D *FlatVertices;
    QVector3D *FlatNormal;

    CreateVertexIndices();
    LoadShaders(":/vshader.Flat.glsl",":/fshader.Flat.glsl");
    CalculateNormal();

    FlatVertices = new QVector4D[offr->num_faces * 3];
    FlatNormal  = new QVector3D[offr->num_faces * 3];

    for(int i = 0; i < offr->num_faces; i++)
    {
        FlatVertices[i*3    ] = vertices[indices[i*3]];
        FlatVertices[i*3 + 1] = vertices[indices[i*3 +1]];
        FlatVertices[i*3 + 2] = vertices[indices[i*3 +1]];

        FlatNormal[i*3    ] = normal[i];
        FlatNormal[i*3 + 1] = normal[i];
        FlatNormal[i*3 + 2] = normal[i];

    }

    /* Create VBO to vertices */
    if (m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(FlatVertices, offr->num_faces*3*sizeof(QVector4D));
    delete []FlatVertices;
    FlatVertices = NULL;

    /* Create VBO to indices */
    if(m_vboNormal) delete m_vboNormal;

    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(FlatNormal, offr->num_faces*3*sizeof(QVector3D));
    delete []FlatNormal;
    FlatNormal = NULL;
    delete []normal;
    normal = NULL;

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);
}

void OpenGL::LoadShaders(std::string const &s1, std::string const &s2){
    m_shaderProgram->release();

    if (m_vertexShader)   delete m_vertexShader;
    if (m_fragmentShader) delete m_fragmentShader;

    m_vertexShader = new QGLShader(QGLShader::Vertex);

    m_fragmentShader = new QGLShader(QGLShader::Fragment);

    if(!m_vertexShader->compileSourceFile(s1.c_str()))
        qWarning() << m_vertexShader->log();

    if(!m_fragmentShader->compileSourceFile(s2.c_str()))
        qWarning() << m_fragmentShader->log();

    if(m_shaderProgram) delete m_shaderProgram;

    m_shaderProgram = new QGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);

    if(!m_shaderProgram->link())
        qWarning() << m_shaderProgram->log() << endl;
    else
        m_shaderProgram->bind();
}

void OpenGL::CalculateNormal(){

    if (normal) delete[] normal;

    normal = new QVector3D[offr->num_faces];

    for(int i = 0; i < offr->num_faces; i++){
        int i0, i1, i2;

        i0 = indices[i*3];
        i1 = indices[i*3 + 1];
        i2 = indices[i*3 + 2];

        QVector3D v1 = (vertices[i1] - vertices[i0]).toVector3D();
        QVector3D v2 = (vertices[i2] - vertices[i0]).toVector3D();

        normal[i] = QVector3D::crossProduct(v1, v2).normalized();
    }
}

void OpenGL::InitializeVBOs(){

    /* Initialize the points that will be draw */
    GetFaces(offr);

    colours = new QVector3D[offr->num_faces * 3];

    /* Initialize vertex's colours */
    for(int i = 0; i < offr->num_faces*3; i++)
        colours[i] = QVector3D(255.0f/255.0f, 215.0f/255.0f, 0.0f/255.0f);
        /*colours[i] = QVector3D(rand()/(float)(RAND_MAX),
                               rand()/(float)(RAND_MAX),
                               rand()/(float)(RAND_MAX));*/

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
    glClearColor(BgRed/255.0, BgGreen/255.0, BgBlue/255.0, 1.0);

    /* Reset matrix */
    ModelView.setToIdentity();
    MatrixProjection.setToIdentity();
    MatrixRotation.setToIdentity();
    MatrixNormal.setToIdentity();

    /* Set lookat */
    ModelView.lookAt(camera.eye, camera.at, camera.up);
    /* Zoom */
    ModelView.scale(zoom/100);
    /* Set rotation */
    ModelView.rotate(trackball.rotation()); //MatrixRotation.rotate(trackball.rotation());
    /* Set NormalMatrix*/
    MatrixNormal = ModelView.normalMatrix();

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

    /* Send matrix to shader */
    m_shaderProgram->setUniformValue("MatrixProjection", MatrixProjection);
    m_shaderProgram->setUniformValue("MatrixTransformation", ModelView);
    //m_shaderProgram->setUniformValue("MatrixModelView", ModelView);
    //m_shaderProgram->setUniformValue("MatrixRotation", MatrixRotation);
    m_shaderProgram->setUniformValue("NormalMatrix", MatrixNormal);

    switch(Shader){
        case 0:
            UseFlatShading();
            break;
        defaut:
            UseFlatShading();
            break;
    }


    /*
    /* VBO of vertex *
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition", GL_FLOAT, 0, 3, 0);

    /* VBO of colours *
    //m_vboColours->bind();
    m_vboIndices->bind();
    m_shaderProgram->enableAttributeArray("vColour");
    m_shaderProgram->setAttributeBuffer("vColour", GL_FLOAT, 0, 3, 0);

    if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLES, 0, offr->num_faces * 3);

    m_vboVertices->release();
    m_vboColours->release();
    */
}

void OpenGL::UseFlatShading(){
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("Pos_Vertice");
    m_shaderProgram->setAttributeBuffer("Pos_Vertice",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    //glDrawArrays( GL_TRIANGLES, 0, offr->num_faces*3 );

    //DEBUG
    //glDrawElements( GL_LINES, 4, GL_UNSIGNED_INT, indices);
    glDrawArrays( GL_TRIANGLE_STRIP , 0, offr->num_faces*3 );

    this->m_vboNormal->release();
    this->m_vboVertices->release();
}

void OpenGL::resizeGL(int width, int height){
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
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

void OpenGL::SetBottom(double n){
    camera.bottom = n;
}

void OpenGL::SetTop(double n){
    camera.top = n;
}

void OpenGL::SetNearplane(double n){
    camera.nearplane = n;
}

void OpenGL::SetFarpalne(double n){
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
    QVector3D Colour;
    OneColour = b;

    if(!Colourgold){
        m_vboColours->bind();
        QVector3D *vColour = (QVector3D*) m_vboColours->map(QGLBuffer::WriteOnly);

        if(OneColour){ /* One colour for each face */
            for(int i = 0; i < offr->num_faces * 3; i++){
                Colour = QVector3D(rand()/(float)(RAND_MAX),
                                   rand()/(float)(RAND_MAX),
                                   rand()/(float)(RAND_MAX));
                vColour[i*3] = Colour;
                vColour[i*3 +1 ] = Colour;
                vColour[i*3 + 2] = Colour;
            }
        }
        else /* Colour gradient for each face*/
            for(int i = 0; i < offr->num_faces * 3; i++)
                vColour[i] = QVector3D(rand()/(float)(RAND_MAX),
                                    rand()/(float)(RAND_MAX),
                                    rand()/(float)(RAND_MAX));
        m_vboColours->unmap();
    }
}

void OpenGL::SetColourgold(bool b){
    Colourgold = b;

    if(Colourgold){
        m_vboColours->bind();
        QVector3D *vColour = (QVector3D*) m_vboColours->map(QGLBuffer::WriteOnly);

        for(int i = 0; i < offr->num_faces * 3; i++)
            vColour[i] = QVector3D(255.0f/255.0f, 215.0f/255.0f, 0.0f/255.0f);
        m_vboColours->unmap();
    }
    else
        SetOnecolour(OneColour);
}

void OpenGL::LoadOFF(){
    QString file = QFileDialog::getOpenFileName(NULL, "Load OFF file", QDir::currentPath() + QString("../"), tr(" *.off *.OFF;;All Files(*)"));
    if (file.length() != 0){
        offr = new OFFReader((char *) file.toStdString().c_str());
        InitializeVBOs();
        updateGL();
    }
    /*else
        QMessageBox::warning(NULL, QString("ERROR!"), QString("").fromUtf8("It is not a file!"), QMessageBox::Ok);*/
}

void OpenGL::SetBgRed(int r){
    BgRed = r;
}

void OpenGL::SetBgGreen(int g){
    BgGreen = g;
}

void OpenGL::SetBgBlue(int b){
    BgBlue = b;
}

void OpenGL::SetShader(int s){
    Shader = s;
}
