"                                                      \n\
\
attribute vec2 vertex;                                 \n\
attribute vec2 txCoord;                                \n\
attribute vec4 vertexColor;                            \n\
\
uniform mat4 WorldMatrix;                              \n\
uniform mat4 ProjectionMatrix;                         \n\
\
varying LOWP vec4 color;                               \n\
varying LOWP vec2 UV;                                  \n\
\
void main()                                            \n\
{                                                      \n\
    gl_Position = ProjectionMatrix * WorldMatrix * vec4( vertex, 0.0, 1.0 );  \n\
\
    UV = txCoord;                                      \n\
    color = vertexColor;                               \n\
}                                                      \n\
"