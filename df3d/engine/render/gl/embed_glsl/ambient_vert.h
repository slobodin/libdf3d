"                                           \n\
\
attribute vec3 a_vertex3;                   \n\
\
uniform mat4 u_worldViewProjectionMatrix;   \n\
uniform LOWP vec4 u_globalAmbient;               \n\
\
varying LOWP vec4 color;                    \n\
\
void main()                                 \n\
{                                           \n\
\
    color = u_globalAmbient; \n\
    color.a = 1.0;                          \n\
\
    gl_Position = u_worldViewProjectionMatrix * vec4( a_vertex3, 1.0 );\n\
}                                           \n\
"