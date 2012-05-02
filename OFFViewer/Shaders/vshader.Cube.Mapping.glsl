attribute   vec4 vPosition;
attribute   vec3 vNormal;
varying vec3 fR;

uniform mat4 MatrixModelView;
uniform mat3 NormalMatrix;
uniform mat4 MatrixProjection;

void main()
{
 gl_Position = MatrixProjection * MatrixModelView * vPosition;

 vec4 eyePosition = MatrixModelView * vPosition;

 vec3 fN = normalize(NormalMatrix * vNormal);
 vec3 fE = eyePosition.xyz;

 fR = reflect(-fE,fN);
}
