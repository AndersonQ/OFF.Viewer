attribute   vec4 vPosition;
attribute   vec3 vNormal;

uniform mat4 MatrixModelView;
uniform vec4 LightPosition;
uniform mat4 MatrixProjection;
uniform mat3 NormalMatrix;

varying  vec3 n;

void main()
{
    n = NormalMatrix * vNormal;
    gl_Position = MatrixProjection * MatrixModelView * vPosition;
}
