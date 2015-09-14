"                                           \n\
\
struct Material                             \n\
{                                           \n\
    vec4 ambient;                           \n\
    vec4 emissive;                          \n\
};                                          \n\
\
attribute vec3 a_vertex3;                   \n\
\
uniform mat4 WorldViewProjectionMatrix;     \n\
uniform vec4 globalAmbient;                 \n\
uniform Material material;                  \n\
\
varying LOWP vec4 color;                    \n\
\
void main()                                 \n\
{                                           \n\
\
    color = material.ambient * globalAmbient + material.emissive; \n\
    color.a = 1.0;                          \n\
\
    gl_Position = WorldViewProjectionMatrix * vec4( a_vertex3, 1.0 );\n\
}                                           \n\
"