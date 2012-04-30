attribute   vec4 Pos_Vertice;
attribute   vec3 vNormal;

varying  vec3 fN;
varying  vec3 fE;
varying  vec3 fL;

uniform mat4 MatrixTransformation;
uniform mat3 NormalMatrix;
uniform vec4 LightPosition;
uniform mat4 MatrixProjection;


void main()
{
 gl_Position = MatrixProjection * MatrixTransformation * Pos_Vertice;

 vec4 eyePosition = MatrixTransformation * Pos_Vertice;
 vec4 eyeLightPos = LightPosition;

 fN = NormalMatrix * vNormal;
 fL = eyeLightPos.xyz - eyePosition.xyz;
 fE = -eyePosition.xyz;

}
