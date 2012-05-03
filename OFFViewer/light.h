#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>

class Light
{
public:
    Light();
    QVector4D position;
    QVector4D ambient;
    QVector4D diffuse;
    QVector4D specular;

    QVector4D position2;
    QVector4D ambient2;
    QVector4D diffuse2;
    QVector4D specular2;

};

#endif // LIGHT_H
