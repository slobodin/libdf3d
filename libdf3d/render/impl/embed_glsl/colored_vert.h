"                                           \n\
\
attribute vec3 a_vertex3;                   \n\
attribute vec2 a_txCoord;                   \n\
attribute vec4 a_vertexColor;               \n\
\
uniform mat4 u_worldViewProjectionMatrix;   \n\
\
uniform LOWP vec4 material_diffuse;                  \n\
\
varying LOWP vec4 color;                    \n\
varying LOWP vec2 UV;                       \n\
\
void main()                                 \n\
{                                           \n\
\
    color = a_vertexColor * material_diffuse; \n\
\
    gl_Position = u_worldViewProjectionMatrix * vec4( a_vertex3, 1.0 );\n\
\
    UV = a_txCoord;                         \n\
}                                           \n\
"
