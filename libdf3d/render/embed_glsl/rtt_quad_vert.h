"                                                      \n\
\
attribute vec3 vertex;                                 \n\
attribute vec2 txCoord;                                \n\
\
varying LOWP vec2 UV;                                  \n\
\
void main()                                            \n\
{                                                      \n\
    gl_Position = vec4( vertex, 1.0 );                 \n\
\
    UV = txCoord;                                      \n\
}                                                      \n\
"