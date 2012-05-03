attribute   vec4 vPosition;
attribute   vec3 vNormal;

varying vec3 v;
varying vec3 N;

uniform mat4 MatrixModelView;
uniform mat3 NormalMatrix;
uniform vec4 LightPosition;
uniform mat4 MatrixProjection;

void main()
{

    v = vec3(MatrixModelView * vPosition);
    N = normalize(NormalMatrix * vNormal);

    gl_Position = MatrixProjection * MatrixModelView * vPosition;
}
