varying vec3 fR;

uniform samplerCube texMap;

void main()
{
    gl_FragColor = textureCube(texMap,fR);
    gl_FragColor.a = 1.0;
}
