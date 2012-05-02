varying vec2 st;

uniform sampler2D texMap_SM;

void main()
{
    gl_FragColor = texture2D(texMap_SM, st);
    gl_FragColor.a = 1.0;
}
