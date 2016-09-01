"                                           \n\
\
uniform LOWP sampler2D diffuseMap;          \n\
\
varying LOWP vec4 color;                    \n\
varying LOWP vec2 UV;                       \n\
\
void main()                                 \n\
{                                           \n\
\
    gl_FragColor = color * texture2D( diffuseMap, UV ); // FIXME: mixing diffuseMap with specular term \n\
}\
"