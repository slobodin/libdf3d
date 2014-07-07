"                                           \n\
\
uniform LOWP sampler2D sceneTexture;        \n\
\
varying LOWP vec2 UV;                       \n\
\
void main()                                 \n\
{                                           \n\
    gl_FragColor = texture2D(sceneTexture, UV); \n\
}\
"