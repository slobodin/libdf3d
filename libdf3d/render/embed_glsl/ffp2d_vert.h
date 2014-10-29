"                                                      \n\
\
attribute vec3 vertex;                                 \n\
attribute vec2 txCoord;                                \n\
attribute vec4 vertexColor;                            \n\
\
uniform mat4 WorldViewProjectionMatrix;                \n\
\
varying LOWP vec4 color;                               \n\
varying LOWP vec2 UV;                                  \n\
\
void main()                                            \n\
{                                                      \n\
    gl_Position = WorldViewProjectionMatrix * vec4( vertex, 1.0 );  \n\
\
    UV = txCoord;                                      \n\
    color = vertexColor;                               \n\
}                                                      \n\
"