"                                           \n\
\
struct Material                             \n\
{                                           \n\
    vec4 diffuse;                           \n\
};                                          \n\
\
attribute vec3 vertex;                      \n\
attribute vec2 txCoord;                     \n\
attribute vec4 vertexColor;                 \n\
\
uniform mat4 WorldViewProjectionMatrix;     \n\
\
uniform Material material;                  \n\
\
varying LOWP vec4 color;                    \n\
varying LOWP vec2 UV;                       \n\
\
void main()                                 \n\
{                                           \n\
\
    color = vertexColor * material.diffuse; \n\
\
    gl_Position = WorldViewProjectionMatrix * vec4( vertex, 1.0 );\n\
\
    UV = vec2(txCoord.x, 1.0 - txCoord.y);              \n\
}                                                       \n\
"