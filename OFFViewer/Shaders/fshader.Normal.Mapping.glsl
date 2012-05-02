varying  vec2 st;
varying  vec3 fL;
varying  vec3 fE;

uniform sampler2D texColorMap;
uniform sampler2D texNormalMap;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
     vec3 N  = normalize((texture2D(texNormalMap,st).rgb*2.0) - 1.0);
     vec3 L  = normalize(fL);
     vec3 E  = normalize(fE);
    vec3 R = normalize(2.0*dot(fL,N)*N - fL);

    float Kd = max(dot(N , L ), 0.0);
    float Ks = pow(max(dot(R , fE ), 0.0), Shininess);

    vec4 diffuse  = Kd * DiffuseProduct;
    vec4 specular = Ks * SpecularProduct;
    vec4 ambient  = AmbientProduct;

    gl_FragColor = (ambient + diffuse + specular)*texture2D(texColorMap,st);
    //gl_FragColor = texture2D(texColorMap,st);
    gl_FragColor.a = 1.0;
}
