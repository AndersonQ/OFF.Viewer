// per-fragment interpolated values from the vertex shader
varying vec3 v;
varying vec3 N;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
    vec3 L = normalize(LightPosition.xyz - v);
    vec3 E = normalize(-v);
    vec3 R = normalize(-reflect(L,N));

    vec4 Iamb = AmbientProduct;
    vec4 Idiff = DiffuseProduct * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);

    vec4 Ispec = SpecularProduct * pow(max(dot(R,E),0.0),0.3 * Shininess);
    Ispec = clamp(Ispec, 0.0, 1.0);

    gl_FragColor = Iamb + Idiff + Ispec;
    gl_FragColor.a = 1.0;
}
