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
uniform mat4 u_worldViewProjectionMatrix;   \n\
uniform vec4 u_globalAmbient;                 \n\
uniform Material material;                  \n\
\
varying LOWP vec4 color;                    \n\
\
void main()                                 \n\
{                                           \n\
\
    color = material.ambient * u_globalAmbient + material.emissive; \n\
    color.a = 1.0;                          \n\
\
    gl_Position = u_worldViewProjectionMatrix * vec4( a_vertex3, 1.0 );\n\
}                                           \n\
"