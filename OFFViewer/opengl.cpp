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
    m_vboTexCoords = NULL;
    m_vboTangents = NULL;
    m_vboBitangents = NULL;
    vertices = NULL;
    indices = NULL;
    normal = NULL;
    tangents = NULL;
    bitangents = NULL;
    texcoords = NULL;

    texColor = texNormal = NULL;

    /* Defautl texture */
    SelectedImage = 0;

    /* Default coorninates mapping */
    SelectedCoord = 0;

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

    wireframe = false;
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
    initFlatShading();
    glClearColor(0.0, 1.0, 0.0, 1.0);

    /* Cullface false */
    glDisable(GL_CULL_FACE);

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
    vertices[0] = QVector4D(0.0, 0.0, 0.0, 1.0);
    //vertices[1] = QVector4D(1.0, 1.0, 0.0, 1.0);
    //vertices[2] = QVector4D(0.5, 0.5, 0.0, 1.0);
    //vertices[3] = QVector4D(-1.0, -1.0, 1.0, 1.0);*/

    /* Create vector to indices */
    if(indices){
        delete[] indices;
        indices = NULL;
    }
    indices = new unsigned int[offr->num_faces * 3];
    int j = 0;
    for(int i = 0; i < offr->num_faces*3; i+=3){
        indices[i   ] = offr->faces[j][0];
        indices[i +1] = offr->faces[j][1];
        indices[i +2] = offr->faces[j][2];
        j++;
    }
    /*DEBUG
    indices[0] = 0;
    //indices[1] = 1;
    //indices[2] = 2;
    //indices[3] = 3;*/
}

void OpenGL::initFlatShading(){

    QVector4D *FlatVertices;
    QVector3D *FlatNormal;

    CreateVertexIndices();
    LoadShaders(":/Shaders/vshader.Flat.glsl",":/Shaders/fshader.Flat.glsl");
    CalculateNormal();

    FlatVertices = new QVector4D[offr->num_faces * 3];
    FlatNormal  = new QVector3D[offr->num_faces * 3];

    for(int i = 0; i < offr->num_faces; i++)
    {
        FlatVertices[i*3    ] = vertices[indices[i*3]   ];
        FlatVertices[i*3 + 1] = vertices[indices[i*3 + 1]];
        FlatVertices[i*3 + 2] = vertices[indices[i*3 + 2]];

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

void OpenGL::UseFlatShading(){
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    glDrawArrays( GL_TRIANGLES, 0, offr->num_faces * 3 );

    //DEBUG GL_TRIANGLE_STRIP
    //glDrawElements( GL_LINES, 4, GL_UNSIGNED_INT, indices);
    //glDrawArrays( GL_TRIANGLE_STRIP , 0, offr->num_faces * 3 );

    this->m_vboNormal->release();
    this->m_vboVertices->release();
}

void OpenGL::initGouraudShading(){

    CreateVertexIndices();
    //CalculateNormal();
    CalculateNormalVertices();

    LoadShaders(":/Shaders/vshader.Gouraud.glsl",":/Shaders/fshader.Gouraud.glsl");

    /* Create VBO to vertices */
    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(vertices , offr->num_vertices * sizeof(QVector4D));
    delete []vertices;
    vertices = NULL;

    /* Create VBO to normal */
    if(m_vboNormal) delete m_vboNormal;
    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(normal, offr->num_vertices * sizeof(QVector3D));
    delete []normal;
    normal = NULL;

    /* Create VBO to indices */
    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices, offr->num_faces * 3 * sizeof(GL_UNSIGNED_INT));
    delete []indices;
    indices = NULL;

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);
}

void OpenGL::UseGouraud(){

    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    m_vboIndices->bind();

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    glDrawElements(GL_TRIANGLES, 3*offr->num_faces, GL_UNSIGNED_INT, 0);

    //DEBUG GL_TRIANGLE_STRIP
    //glDrawElements(GL_POINTS , 1, GL_UNSIGNED_INT, (GLvoid *) indices);
    //glDrawArrays( GL_TRIANGLE_STRIP , 0, 3 * ( offr->num_faces ) );

    m_vboVertices->release();
    m_vboIndices->release();
    m_vboNormal->release();
}

void OpenGL::initPhong(){

    CreateVertexIndices();
    //CalculateNormal();
    CalculateNormalVertices();

    LoadShaders(":/Shaders/vshader.Phong.glsl",":/Shaders/fshader.Phong.glsl");

    /* Create VBO to vertices */
    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(vertices , offr->num_vertices * sizeof(QVector4D));
    delete []vertices;
    vertices = NULL;

    /* Create VBO to normal */
    if(m_vboNormal) delete m_vboNormal;
    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(normal, offr->num_vertices * sizeof(QVector3D));
    delete []normal;
    normal = NULL;

    /* Create VBO to indices */
    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices, offr->num_faces * 3 * sizeof(GL_UNSIGNED_INT));
    delete []indices;
    indices = NULL;

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);
}

void OpenGL::UsePhong(){
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    m_vboIndices->bind();

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    glDrawElements(GL_TRIANGLES, 3*( offr->num_faces ), GL_UNSIGNED_INT, 0);

    m_vboVertices->release();
    m_vboIndices->release();
}

void OpenGL::initPhongHalf(){
    CreateVertexIndices();
    //CalculateNormal();
    CalculateNormalVertices();

    LoadShaders(":/Shaders/vshader.Phong.Half.glsl",":/Shaders/fshader.Phong.Half.glsl");

    //CalculateNormal();
    CalculateNormalVertices();


    /* Create VBO to vertices */
    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(vertices , offr->num_vertices * sizeof(QVector4D));
    delete []vertices;
    vertices = NULL;

    /* Create VBO to normal */
    if(m_vboNormal) delete m_vboNormal;
    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(normal, offr->num_vertices * sizeof(QVector3D));
    delete []normal;
    normal = NULL;

    /* Create VBO to indices */
    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices, offr->num_faces * 3 * sizeof(GL_UNSIGNED_INT));
    delete []indices;
    indices = NULL;

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

}

void OpenGL::UsePhongHalf(){
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    m_vboIndices->bind();

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    glDrawElements(GL_TRIANGLES, 3*( offr->num_faces ), GL_UNSIGNED_INT, 0);

    //DEBUG GL_TRIANGLE_STRIP
    //glDrawElements(GL_POINTS , 1, GL_UNSIGNED_INT, (GLvoid *) indices);
    //glDrawArrays( GL_TRIANGLES , 0, 3*( offr->num_faces ) );


    m_vboVertices->release();
    m_vboIndices->release();
}

void OpenGL::initCartoon(){
    CreateVertexIndices();
    //CalculateNormal();
    CalculateNormalVertices();

    LoadShaders(":/Shaders/vshader.Cartoon.glsl",":/Shaders/fshader.Cartoon.glsl");    

    /* Create VBO to vertices */
    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(vertices , offr->num_vertices * sizeof(QVector4D));
    delete []vertices;
    vertices = NULL;

    /* Create VBO to normal */
    if(m_vboNormal) delete m_vboNormal;
    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(normal, offr->num_vertices * sizeof(QVector3D));
    delete []normal;
    normal = NULL;

    /* Create VBO to indices */
    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices, offr->num_faces * 3 * sizeof(GL_UNSIGNED_INT));
    delete []indices;
    indices = NULL;

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    /*QVector4D diffuse_product  = light.diffuse * material.diffuse;

    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);*/
}

void OpenGL::UseCartoon(){
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    m_vboIndices->bind();

    QVector4D ambient_product  = light.ambient * material.ambient;
    QVector4D diffuse_product  = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    glDrawElements(GL_TRIANGLES, 3*( offr->num_faces ), GL_UNSIGNED_INT, 0);

    //DEBUG GL_TRIANGLE_STRIP
    //glDrawElements(GL_POINTS , 1, GL_UNSIGNED_INT, (GLvoid *) indices);
    //glDrawArrays( GL_TRIANGLE_STRIP , 0, 3 * ( offr->num_faces ) );

    m_vboVertices->release();
    m_vboIndices->release();
}


void OpenGL::initSimpleTexMapping(){
    CreateVertexIndices();
    //CalculateNormal();
    CalculateNormalVertices();

    ChooseTexture();
    ChooseCoordinates();

    LoadShaders(":/Shaders/vshader.Simple.Tex.Mapping.glsl",":/Shaders/fshader.Simple.Tex.Mapping.glsl");

    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(vertices , offr->num_vertices*sizeof(QVector4D));
    delete []vertices;
    vertices = NULL;

    if(m_vboTexCoords) delete m_vboTexCoords;
    m_vboTexCoords = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboTexCoords->create();
    m_vboTexCoords->bind();
    m_vboTexCoords->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboTexCoords->allocate(texcoords , offr->num_vertices*sizeof(QVector2D));
    delete []texcoords;
    texcoords =NULL;

    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices , offr->num_faces*3*sizeof(GL_UNSIGNED_INT));
    delete []indices;
    indices =NULL;

    glActiveTexture(GL_TEXTURE0);
    QGLWidget::bindTexture((*texColor));
    m_shaderProgram->setUniformValue("texMap_SM",0);
}

void OpenGL::UseSimpleTexMapping(){
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboTexCoords->bind();
    m_shaderProgram->enableAttributeArray("vTexCoords_SM");
    m_shaderProgram->setAttributeBuffer("vTexCoords_SM",GL_FLOAT,0,2,0);

    m_vboIndices->bind();

    glDrawElements(GL_TRIANGLES, 3*offr->num_faces, GL_UNSIGNED_INT, 0);

    //DEBUG GL_TRIANGLE_STRIP
    //glDrawElements(GL_POINTS , 1, GL_UNSIGNED_INT, (GLvoid *) indices);
    //glDrawArrays( GL_TRIANGLE_STRIP , 0, 3 * ( offr->num_faces ) );

    m_vboVertices->release();
    m_vboIndices->release();
    m_vboTexCoords->release();
}

void OpenGL::initCubeMapping(){
    QImage img[6];

    CreateVertexIndices();
    //CalculateNormal();
    CalculateNormalVertices();

    LoadShaders(":/Shaders/vshader.Cube.Mapping.glsl",":/Shaders/fshader.Cube.Mapping.glsl");

    img[0] = QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/negx.jpg","JPG");
    img[1] = QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/negy.jpg","JPG");
    img[2] = QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/negz.jpg","JPG");
    img[3] = QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/posx.jpg","JPG");
    img[4] = QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/posy.jpg","JPG");
    img[5] = QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/posz.jpg","JPG");

    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(vertices, offr->num_vertices * sizeof(QVector4D));
    delete []vertices;
    vertices =NULL;

    if(m_vboNormal) delete m_vboNormal;
    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(normal, offr->num_vertices * sizeof(QVector3D));
    delete []normal;
    normal = NULL;

    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices , offr->num_faces * 3 * sizeof(GL_UNSIGNED_INT));
    delete []this->indices;
    indices = NULL;

    m_shaderProgram->setUniformValue("texMap",0);

    glActiveTexture(GL_TEXTURE0);
    QGLWidget::bindTexture(img[0],GL_TEXTURE_CUBE_MAP);

    QGLWidget::bindTexture(img[0],GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    QGLWidget::bindTexture(img[4],GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    QGLWidget::bindTexture(img[2],GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    QGLWidget::bindTexture(img[3],GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    QGLWidget::bindTexture(img[1],GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    QGLWidget::bindTexture(img[5],GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
}

void OpenGL::UseCubeMapping(){
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition", GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal", GL_FLOAT,0,3,0);

    m_vboIndices->bind();

    glDrawElements(GL_TRIANGLES, 3 * offr->num_faces, GL_UNSIGNED_INT, 0);

    //DEBUG GL_TRIANGLE_STRIP
    //glDrawElements(GL_POINTS , 1, GL_UNSIGNED_INT, (GLvoid *) indices);
    //glDrawArrays( GL_TRIANGLE_STRIP , 0, 3 * ( offr->num_faces ) );

    m_vboVertices->release();
    m_vboIndices->release();

}

void OpenGL::initNormalMapping(){
    CreateVertexIndices();
    CalculateNormalVertices();

    //if(texColor) delete texColor;
    if(texNormal) delete texNormal;

    SelectedImage = 2;
    ChooseTexture();
    //texNormal = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/fieldstone-c.jpg","JPG");
    texNormal = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/fieldstone-n.jpg","JPG");
    ChooseCoordinates();
    genTangentFrames();

    LoadShaders(":/Shaders/vshader.Normal.Mapping.glsl",":/Shaders/fshader.Normal.Mapping.glsl");

    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(this->vertices , offr->num_vertices *sizeof(QVector4D));
    delete []vertices;
    vertices =NULL;


    if(m_vboNormal) delete m_vboNormal;
    m_vboNormal = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormal->create();
    m_vboNormal->bind();
    m_vboNormal->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormal->allocate(this->normal , offr->num_vertices *sizeof(QVector3D));
    delete []normal;
    normal =NULL;

    if(m_vboTangents) delete m_vboTangents;
    m_vboTangents = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboTangents->create();
    m_vboTangents->bind();
    m_vboTangents->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboTangents->allocate(this->tangents , offr->num_vertices *sizeof(QVector3D));
    delete []tangents;
    tangents =NULL;

    if(m_vboBitangents) delete m_vboBitangents;
    m_vboBitangents = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboBitangents->create();
    m_vboBitangents->bind();
    m_vboBitangents->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboBitangents->allocate(this->bitangents , offr->num_vertices *sizeof(QVector3D));
    delete []bitangents;
    bitangents =NULL;


    if(m_vboTexCoords) delete m_vboTexCoords;
    m_vboTexCoords = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboTexCoords->create();
    m_vboTexCoords->bind();
    m_vboTexCoords->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboTexCoords->allocate(this->texcoords , offr->num_vertices *sizeof(QVector2D));
    delete []texcoords;
    texcoords =NULL;


    if(m_vboIndices) delete m_vboIndices;
    m_vboIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    m_vboIndices->create();
    m_vboIndices->bind();
    m_vboIndices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboIndices->allocate(indices ,  offr->num_faces * 3 * sizeof(GL_UNSIGNED_INT));
    delete []this->indices;
    indices =NULL;

    QVector4D ambient_product = light.ambient * material.ambient;
    QVector4D diffuse_product = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    m_shaderProgram->setUniformValue("texColorMap",0);
    m_shaderProgram->setUniformValue("texNormalMap",1);

    glActiveTexture(GL_TEXTURE0);
    QGLWidget::bindTexture((*texColor));
    m_shaderProgram->setUniformValue("texColorMap",0);

    glActiveTexture(GL_TEXTURE1);
    QGLWidget::bindTexture((*texNormal));
    m_shaderProgram->setUniformValue("texNormalMap",1);
}

void OpenGL::UseNormalMapping(){
    m_vboVertices->bind();
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);

    m_vboNormal->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);

    m_vboTangents->bind();
    m_shaderProgram->enableAttributeArray("vTangent");
    m_shaderProgram->setAttributeBuffer("vTangent",GL_FLOAT,0,3,0);

    m_vboBitangents->bind();
    m_shaderProgram->enableAttributeArray("vBitangent");
    m_shaderProgram->setAttributeBuffer("vBitangent",GL_FLOAT,0,3,0);

    m_shaderProgram->setUniformValue("Shininess", material.shininess);

    m_vboTexCoords->bind();
    m_shaderProgram->enableAttributeArray("vTexCoords");
    m_shaderProgram->setAttributeBuffer("vTexCoords",GL_FLOAT,0,2,0);

    m_vboIndices->bind();

    QVector4D ambient_product = light.ambient * material.ambient;
    QVector4D diffuse_product = light.diffuse * material.diffuse;
    QVector4D specular_product = light.specular * material.specular;

    m_shaderProgram->setUniformValue("LightPosition",light.position);
    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("Shininess",material.shininess);

    glDrawElements(GL_TRIANGLES, offr->num_faces * 3, GL_UNSIGNED_INT, 0);

    m_vboVertices->release();
    m_vboIndices->release();
    m_vboTexCoords->release();
}

void OpenGL::ChooseTexture(){

    if (texColor) delete texColor;

    switch(SelectedImage)
    {
        case 0:
            texColor = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/stone_color128.jpg","JPG");
            break;
        case 1:
            texColor = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/BTStone.bmp","BMP");
            break;
        case 2:
            texColor = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/fieldstone-c.jpg","JPG");
            break;
        case 3:
            texColor = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/checkerboard.png","PNG");
            break;
        case 4:
            texColor = new QImage("/media/Mokona/UFABC/10-Quad/Computacao.Grafica/Proj2/OFF.Viewer/OFFViewer/Textures/ufabc.jpg","JPG");
            break;
        default:
            printf("ERROR: SelectedImage = %d\n", SelectedImage);
            exit(0);
            break;
    }

}

void OpenGL::ChooseCoordinates(){
    switch(SelectedCoord)
    {
        case 0:
            genTexCoordsCylinder();
            break;
        case 1:
            genTexCoordsSphere();
            break;
        case 2:
            genTexCoordsTriangles();
            break;
        default:
            printf("ERROR: SelectedCoord = %d\n", SelectedImage);
            exit(0);
            break;
    }
}

void OpenGL::genTexCoordsCylinder(){

    double max_x = -1e10, min_x = 1e10;
    double max_y = -1e10, min_y = 1e10;
    double max_z = -1e10, min_z = 1e10;
    double u, v, x;

    if(texcoords)
        delete[] texcoords;

    texcoords = new QVector2D[offr->num_vertices];

    for(int i = 0; i < offr->num_vertices; i++){
        if(vertices[i].x() > max_x) max_x = vertices[i].x();
        if(vertices[i].x() < min_x) min_x = vertices[i].x();
        if(vertices[i].y() > max_y) max_y = vertices[i].y();
        if(vertices[i].y() < min_y) min_y = vertices[i].y();
        if(vertices[i].z() > max_z) max_z = vertices[i].z();
        if(vertices[i].z() < min_z) min_z = vertices[i].z();
    }

    for (int i = 0; i < offr->num_vertices; i++){
        x = 2*(vertices[i].x() - min_x)/(max_x-min_x) - 1;
        u = ::acos(x)/(3.14);
        v = (vertices[i].y() - min_y)/(max_y-min_y);
        texcoords[i] = QVector2D(u,v);
    }

}

void OpenGL::genTexCoordsSphere()
{
    double max_x = -1e10, min_x = 1e10;
    double max_y = -1e10, min_y = 1e10;
    double max_z = -1e10, min_z = 1e10;
    double u,v;
    int i;

    if(texcoords) delete [] texcoords;
    texcoords = new QVector2D [offr->num_vertices];

    for(i = 0; i < offr->num_vertices; i++)
    {
        if(vertices[i].x() > max_x) max_x = vertices[i].x();
        if(vertices[i].x() < min_x) min_x = vertices[i].x();
        if(vertices[i].y() > max_y) max_y = vertices[i].y();
        if(vertices[i].y() < min_y) min_y = vertices[i].y();
        if(vertices[i].z() > max_z) max_z = vertices[i].z();
        if(vertices[i].z() < min_z) min_z = vertices[i].z();
    }

    for(i = 0; i < offr->num_vertices;i++)
    {
        //http://www.mvps.org/directx/articles/spheremap.htm
        u = ::asin(normal[i].x())/3.14 + 0.5;
        v = ::asin(normal[i].y())/3.14 + 0.5;

        texcoords[i] = QVector2D(u,v);
    }
}

void OpenGL::genTexCoordsTriangles()
{
    int i;

    if(texcoords) delete [] texcoords;
    texcoords = new QVector2D [offr->num_vertices];

    for(i = 0; i < offr->num_vertices;i++)
    {
        texcoords[i] = QVector2D((i+1.0)/(offr->num_vertices),(i+1.0)/(offr->num_vertices));
    }
}

//http://www.terathon.com/code/tangent.html
void OpenGL::genTangentFrames()
{
    if(tangents) delete [] tangents;
    if(bitangents) delete []bitangents;

    tangents = new QVector3D[offr->num_vertices];
    bitangents = new QVector3D[offr->num_vertices];


    for (int i = 0; i < offr->num_faces; i++)
    {
        unsigned int i1 = indices[3*i];
        unsigned int i2 = indices[3*i+1];
        unsigned int i3 = indices[3*i+2];

        QVector4D v1 = vertices[i1];
        QVector4D v2 = vertices[i2];
        QVector4D v3 = vertices[i3];

        QVector2D w1 = texcoords[i1];
        QVector2D w2 = texcoords[i2];
        QVector2D w3 = texcoords[i3];

        double x1 = v2.x() - v1.x();
        double x2 = v3.x() - v1.x();
        double y1 = v2.y() - v1.y();
        double y2 = v3.y() - v1.y();
        double z1 = v2.z() - v1.z();
        double z2 = v3.z() - v1.z();

        double s1 = w2.x() - w1.x();
        double s2 = w3.x() - w1.x();
        double t1 = w2.y() - w1.y();
        double t2 = w3.y() - w1.y();

        double r = 1.0F / (s1 * t2 - s2 * t1);
        QVector3D sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                       (t2 * z1 - t1 * z2) * r);
        QVector3D tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                       (s1 * z2 - s2 * z1) * r);

        tangents[i1] += sdir;
        tangents[i2] += sdir;
        tangents[i3] += sdir;

        bitangents[i1] += tdir;
        bitangents[i2] += tdir;
        bitangents[i3] += tdir;

    }

    for (int a = 0; a < offr->num_vertices; a++)
    {
        const QVector3D& n = normal[a];
        const QVector3D& t = tangents[a];

        // Gram-Schmidt orthogonalize
        tangents[a] = (t - n * QVector3D::dotProduct(n, t)).normalized();

        // Calculate handedness
        double hand  = (QVector3D::dotProduct(QVector3D::crossProduct(n, t), bitangents[a]) < 0.0F) ? -1.0F : 1.0F;
        bitangents[a] = QVector3D::crossProduct(normal[a], tangents[a])*hand;
    }
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

void OpenGL::CalculateNormalVertices(){

    if (normal) delete[] normal;

    normal = new QVector3D[offr->num_vertices];

    for(int i = 0; i < offr->num_vertices; i++){
        normal[i] = QVector3D(0,0,0);
    }

    for(int i = 0; i < offr->num_faces; i++){
        int i0, i1, i2;

        i0 = indices[i*3];
        i1 = indices[i*3 + 1];
        i2 = indices[i*3 + 2];

        QVector3D v1 = (vertices[i1] - vertices[i0]).toVector3D();
        QVector3D v2 = (vertices[i2] - vertices[i0]).toVector3D();

        QVector3D n = QVector3D::crossProduct(v1, v2).normalized();
        normal[i0] += n;
        normal[i1] += n;
        normal[i2] += n;
    }

    for(int i = 0; i < offr->num_vertices; i++){
        normal[i].normalize();
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
    m_shaderProgram->setUniformValue("MatrixModelView", ModelView);
    //m_shaderProgram->setUniformValue("MatrixRotation", MatrixRotation);
    m_shaderProgram->setUniformValue("NormalMatrix", MatrixNormal);

    if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    switch(Shader){
        case 0:
            UseFlatShading();
            break;
        case 1:
            UseGouraud();
            break;
        case 2:
            UsePhong();
            break;
        case 3:
            UsePhongHalf();
            break;
        case 4:
            UseCartoon();
            break;
        case 5:
            UseSimpleTexMapping();
            break;
        case 6:
            UseCubeMapping();
            break;
        case 7:
            UseNormalMapping();
            break;
        default:
            //UseFlatShading();
            printf("ERROR Shader recived %d\n", Shader);
            fflush(stdout);
            exit(0);
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

void OpenGL::ChangeShader(int s){
    switch(s){
        case 0:
            initFlatShading();
            break;
        case 1:
            initGouraudShading();
            break;
        case 2:
            initPhong();
            break;
        case 3:
            initPhongHalf();
            break;
        case 4:
            initCartoon();
            break;
        case 5:
            initSimpleTexMapping();
            break;
        case 6:
            initCubeMapping();
            break;
        case 7:
            initNormalMapping();
            break;
        default:
            //initFlatShading();
            printf("ERROR ChangeShader recived %d\n", s);
            fflush(stdout);
            exit(0);
            break;
    }
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
    ChangeShader(s);
}

void OpenGL::SetCoordnates(int c){
    SelectedCoord = c;
    ChangeShader(Shader);
}

void OpenGL::SetTexture(int t){
    SelectedImage = t;
    ChangeShader(Shader);
}

void OpenGL::SetMatAmbx(int i){
    material.ambient.setX(i/255.0);
    updateGL();
}

void OpenGL::SetMatAmby(int i){
    material.ambient.setY(i/255.0);
    updateGL();
}
void OpenGL::SetMatAmbz(int i){
    material.ambient.setZ(i/255.0);
    updateGL();
}
void OpenGL::SetMatDifx(int i){
    material.diffuse.setX(i/255.0);
    updateGL();
}
void OpenGL::SetMatDify(int i){
    material.diffuse.setX(i/255.0);
    updateGL();
}
void OpenGL::SetMatDifz(int i){
    material.diffuse.setX(i/255.0);
    updateGL();
}
void OpenGL::SetMatSpecx(int i){
    material.specular.setX(i/255.0);
    updateGL();
}
void OpenGL::SetMatSpecy(int i){
    material.specular.setX(i/255.0);
    updateGL();
}
void OpenGL::SetMatSpecz(int i){
    material.specular.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightAmbx(int i){
    light.ambient.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightAmby(int i){
    light.ambient.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightAmbz(int i){
    light.ambient.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightDifx(int i){
    light.diffuse.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightDify(int i){
    light.diffuse.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightDifz(int i){
    light.diffuse.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightSpecx(int i){
    light.specular.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightSpecy(int i){
    light.specular.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightSpecz(int i){
    light.specular.setX(i/255.0);
    updateGL();
}
void OpenGL::SetLightEyecx(double i){
    light.position.setX(i);
    updateGL();
}
void OpenGL::SetLightEyecy(double i){
    light.position.setX(i);
    updateGL();
}
void OpenGL::SetLightEyecz(double i){
    light.position.setX(i);
    updateGL();
}

