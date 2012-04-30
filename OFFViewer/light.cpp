#include "light.h"

Light::Light()
{
    position = QVector4D(3, 3, 3, 0.0);
    ambient = QVector4D(0.2, 0.2, 0.2, 1.0);
    diffuse = QVector4D(1.0, 1.0, 1.0, 1.0);
    specular = QVector4D(1.0, 1.0, 1.0, 1.0);
}
