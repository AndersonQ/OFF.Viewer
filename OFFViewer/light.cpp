#include "light.h"

Light::Light()
{
    position = position2 = QVector4D(3, 3, 3, 0.0);
    ambient = ambient2 = QVector4D(0.2, 0.2, 0.2, 1.0);
    diffuse = diffuse2 = QVector4D(1.0, 1.0, 1.0, 1.0);
    specular = specular2 = QVector4D(1.0, 1.0, 1.0, 1.0);

    /*position2 = QVector4D(0, -1, 0, 0.0);
    ambient2 = QVector4D(0.2, 0.2, 0.2, 1.0);
    diffuse2 = QVector4D(1.0, 1.0, 1.0, 1.0);
    specular2 = QVector4D(1.0, 1.0, 1.0, 1.0);*/
}
