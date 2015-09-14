"                                                      \n\
\
attribute vec3 a_vertex3;                               \n\
attribute vec2 a_txCoord;                              \n\
\
varying LOWP vec2 UV;                                  \n\
\
void main()                                            \n\
{                                                      \n\
    gl_Position = vec4( a_vertex3, 1.0 );               \n\
\
    UV = a_txCoord;                                    \n\
}                                                      \n\
"