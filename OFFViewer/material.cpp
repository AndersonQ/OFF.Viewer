#include "material.h"

Material::Material()
{
    ambient = QVector4D(1.0, 0.0, 1.0, 1.0);
    diffuse = QVector4D(1.0, 0.8, 0.0, 1.0);
    specular = QVector4D(1.0, 0.8, 0.0, 1.0);
    shininess = 10000.0;
}
