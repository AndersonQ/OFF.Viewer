attribute vec4 vPosition;
attribute vec3 vNormal;
varying vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 MatrixModelView;
uniform mat4 MatrixProjection;
//uniform mat4 MatrixRotation;
uniform mat3 NormalMatrix;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (MatrixModelView * vPosition).xyz;

    //vec3 L = normalize( (MatrixModelView*LightPosition).xyz - pos );
    vec3 L = normalize(LightPosition.xyz - pos);

    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( NormalMatrix*vNormal);

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;

    if( dot(L, N) < 0.0 )
    {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    }

    gl_Position = MatrixProjection * MatrixModelView * vPosition;

    color = ambient + diffuse + specular;
    color.a = 1.0;
}
