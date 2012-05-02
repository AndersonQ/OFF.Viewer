attribute   vec4 vPosition;
attribute   vec2 vTexCoords_SM;

varying vec2 st;

uniform mat4 MatrixModelView;
uniform mat4 MatrixProjection;

void main()
{
    gl_Position = MatrixProjection * MatrixModelView * vPosition;
    //gl_ModelViewProjectionMatrix * vPosition;
    st = vTexCoords_SM;
}
