"                                           \n\
\
struct Material                             \n\
{                                           \n\
    vec4 diffuse;                           \n\
    vec4 specular;                          \n\
    float shininess;                        \n\
};                                          \n\
\
struct Light                                \n\
{                                           \n\
    vec3 diffuse;                           \n\
    vec3 specular;                          \n\
    // NOTE: Position in view space.        \n\
    vec4 position;                          \n\
};                                          \n\
\
attribute vec3 vertex;                      \n\
attribute vec3 normal;                      \n\
attribute vec2 txCoord;                     \n\
\
uniform mat4 WorldViewMatrix;               \n\
uniform mat4 WorldViewProjectionMatrix;     \n\
uniform mat3 NormalMatrix;                  \n\
\
uniform Material material;                  \n\
uniform vec4 globalAmbient;                 \n\
uniform Light current_light;                \n\
\
varying LOWP vec4 color;                    \n\
varying LOWP vec2 UV;                       \n\
\
// Vertex in view space                     \n\
vec3 P;                                     \n\
// Vertex normal (view space)               \n\
vec3 N;                                     \n\
// Vector to the camera. NOTE: camera position at (0, 0, 0) \n\
vec3 V;                                     \n\
\
void illuminate()                                           \n\
{                                                           \n\
    // Vector to the light source                           \n\
    vec3 L = normalize( current_light.position.xyz );       \n\
\
    // Compute diffuse component.                           \n\
    // FIXME: L is inverted because only dir lights.        \n\
    float lambertDiffuse = max(dot(N, -L), 0.0);            \n\
    vec3 diffuse = material.diffuse.rgb * current_light.diffuse * lambertDiffuse;\n\
\
    // Compute specular light. NOTE: Phong model.           \n\
    vec3 r = reflect( L, N );                               \n\
    float phongSpecular = pow(max(dot(r, V), 0.0), material.shininess); \n\
    vec3 specular = material.specular.rgb * current_light.specular * phongSpecular; \n\
\
    color.rgb += diffuse + specular;                        \n\
}                                                           \n\
\
void main()                                             \n\
{                                                       \n\
    N = normalize( NormalMatrix * normal );             \n\
    P = (WorldViewMatrix * vec4(vertex, 1.0)).xyz;                    \n\
    V = normalize( -P );                                \n\
\
    color = vec4(0.0, 0.0, 0.0, 1.0);                   \n\
\
    illuminate();                                       \n\
\
    gl_Position = WorldViewProjectionMatrix * vec4(vertex, 1.0);     \n\
\
    UV = vec2(txCoord.x, 1.0 - txCoord.y);              \n\
}                                                       \n\
"
