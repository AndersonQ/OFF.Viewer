// per-fragment interpolated values from the vertex shader
varying  vec3 fN;
varying  vec3 fL;
varying  vec3 fE;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
     vec3 N  = normalize(fN);
     vec3 L  = normalize(fL);
     vec3 E  = normalize(fE);
     vec3 H  = normalize(L+E);

    float Kd = max(dot(N , L ), 0.0);
    float Ks = pow(max(dot(L , E ), 0.0), Shininess);

    vec4 diffuse  = Kd * DiffuseProduct;
    vec4 specular = Ks * SpecularProduct;
    vec4 ambient  = AmbientProduct;

    gl_FragColor = ambient + diffuse + specular;
    gl_FragColor.a = 1.0;

}
