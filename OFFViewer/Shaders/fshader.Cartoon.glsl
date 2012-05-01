uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

varying  vec3 n;

void main()
{
    vec4 color;

    color = AmbientProduct*1.5 + DiffuseProduct + SpecularProduct;

    float intensity = dot(normalize(LightPosition.xyz), normalize(n));

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

    gl_FragColor = color;
    gl_FragColor.a = 1.0;
}
