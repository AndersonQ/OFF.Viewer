// per-fragment interpolated values from the vertex shader
varying vec3 v;
varying vec3 N;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

uniform vec4 AmbientProduct2, DiffuseProduct2, SpecularProduct2;
uniform vec4 LightPosition2;
uniform float Shininess2;

void main()
{
    vec4 finalColor1, finalColor2; //= vec4(0.0, 0.0, 0.0, 0.0);
    vec3 L = normalize(LightPosition.xyz - v);
    vec3 E = normalize(-v);
    vec3 R = normalize(-reflect(L,N));

    vec4 Iamb = AmbientProduct;
    vec4 Idiff = DiffuseProduct * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);

    vec4 Ispec = SpecularProduct * pow(max(dot(R,E),0.0),0.3 * Shininess);
    Ispec = clamp(Ispec, 0.0, 1.0);

    finalColor1 = Iamb + Idiff + Ispec;
    //gl_FragColor = Iamb + Idiff + Ispec;

    //light2

    L = normalize(LightPosition2.xyz - v);
    E = normalize(-v);
    R = normalize(-reflect(L,N));

    Iamb = AmbientProduct2;
    Idiff = DiffuseProduct2 * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);

    Ispec = SpecularProduct2 * pow(max(dot(R,E),0.0),0.3 * Shininess2);
    Ispec = clamp(Ispec, 0.0, 1.0);

    finalColor2 = Iamb + Idiff + Ispec;

    gl_FragColor = finalColor1 + finalColor2;
    gl_FragColor.a = 1.0;
}
