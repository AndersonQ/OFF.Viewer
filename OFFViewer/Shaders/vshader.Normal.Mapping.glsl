attribute   vec4 vPosition;
attribute   vec3 vNormal;
attribute   vec3 vTangent;
attribute   vec3 vBitangent;
attribute   vec2 vTexCoords;

varying  vec2 st;
varying  vec3 fE;
varying  vec3 fL;

uniform mat4 MatrixModelView;
uniform mat3 NormalMatrix;
uniform vec4 LightPosition;
uniform mat4 MatrixProjection;

void main()
{
    gl_Position = MatrixProjection * MatrixModelView * vPosition;

    vec4 eyePosition = MatrixModelView * vPosition;
    vec4 eyeLightPos =  LightPosition;

    vec3 T = NormalMatrix * vTangent;
    //Bitangent could be computed here
    vec3 B = NormalMatrix * vBitangent;
    vec3 N = NormalMatrix * vNormal;

   mat3 TBN = mat3(T.x,B.x,N.x,
                   T.y,B.y,N.y,
                   T.z,B.z,N.z);

    fL = TBN*(eyeLightPos.xyz - eyePosition.xyz);

    fE = TBN*(-eyePosition.xyz);

    st = vTexCoords;
}
