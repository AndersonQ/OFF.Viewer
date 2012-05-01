attribute   vec4 vPosition;
attribute   vec3 vNormal;

varying  vec3 fN;
varying  vec3 fE;
varying  vec3 fL;

uniform mat4 MatrixModelView;
uniform mat3 NormalMatrix;
uniform vec4 LightPosition;
uniform mat4 MatrixProjection;


void main()
{
 gl_Position =  MatrixProjection * MatrixModelView * vPosition; //MatrixProjection * MatrixModelView * vPosition;
 //gl_ModelViewProjectionMatrix

 vec4 eyePosition = MatrixModelView * vPosition;
 vec4 eyeLightPos = LightPosition;

 fN = NormalMatrix * vNormal;
 fL = eyeLightPos.xyz - eyePosition.xyz;
 fE = -eyePosition.xyz;

}
