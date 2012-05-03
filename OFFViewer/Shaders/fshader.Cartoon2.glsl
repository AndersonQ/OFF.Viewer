uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

uniform vec4 AmbientProduct2, DiffuseProduct2, SpecularProduct2;
uniform vec4 LightPosition2;
uniform float Shininess2;

varying  vec3 n;

void main()
{
    vec4 color, color2;

    color = AmbientProduct*1.5 + DiffuseProduct + SpecularProduct;

    color2 = (AmbientProduct2 + DiffuseProduct2 + SpecularProduct2)*0.5;

    float intensity = dot(normalize(LightPosition.xyz), normalize(n));

    float intensity2 = dot(normalize(LightPosition2.xyz), normalize(n));

    if(intensity > 0.8)
        color *= 0.9;
    else if(intensity > 0.6)
        color *= 0.6;
    else if(intensity > 0.4)
        color *= 0.4;
    else if(intensity > 0.2)
        color *= 0.2;
    else
        color *= 0.1;

    if(intensit2y > 0.8)
        color2 *= 0.9;
    else if(intensity > 0.6)
        color2 *= 0.6;
    else if(intensity > 0.4)
        color2 *= 0.4;
    else if(intensity > 0.2)
        color2 *= 0.2;
    else
        color2 *= 0.1;

    gl_FragColor = (color * normalize(LightPosition)) + 0.5*(color2 * normalize(LightPosition2));
    gl_FragColor.a = 1.0;
}
