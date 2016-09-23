"                                           \n\
\
struct Light                                \n\
{                                           \n\
    vec4 color;                          \n\
    // NOTE: Position in view space.        \n\
    vec4 position;                          \n\
};                                          \n\
\
attribute vec3 a_vertex3;                    \n\
attribute vec3 a_normal;                    \n\
attribute vec2 a_txCoord;                   \n\
\
uniform mat4 u_worldViewMatrix;               \n\
uniform mat4 u_worldViewProjectionMatrix;     \n\
uniform mat3 u_normalMatrix;                  \n\
\
uniform LOWP vec4 material_diffuse;                  \n\
uniform LOWP vec4 material_specular;                  \n\
uniform MEDIUMP float material_shininess;                  \n\
uniform Light current_light;                \n\
\
varying LOWP vec4 color;                    \n\
varying LOWP vec2 UV;                       \n\
\
// Vertex in view space                     \n\
vec3 P;                                     \n\
// Vertex normal (view space)               \n\
MEDIUMP vec3 N;                                     \n\
// Vector to the camera. NOTE: camera position at (0, 0, 0) \n\
MEDIUMP vec3 V;                                     \n\
\
vec4 illuminate()                                           \n\
{                                                           \n\
    // Vector to the light source                           \n\
    vec3 L = normalize( current_light.position.xyz );       \n\
\
    // Compute diffuse component.                           \n\
    // FIXME: L is inverted because only dir lights.        \n\
    float lambertDiffuse = max(dot(N, -L), 0.0);     \n\
    vec4 diffuse = material_diffuse * current_light.color * lambertDiffuse;\n\
\
    // Compute specular light. NOTE: Phong model.           \n\
    vec3 r = reflect( L, N );                               \n\
    float phongSpecular = pow(clamp(dot(r, V), 0.0, 1.0), material_shininess); \n\
    vec4 specular = material_specular * current_light.color * phongSpecular; \n\
\
    return diffuse + specular;                             \n\
}                                                           \n\
\
void main()                                             \n\
{                                                       \n\
    N = normalize( u_normalMatrix * a_normal );           \n\
    P = (u_worldViewMatrix * vec4(a_vertex3, 1.0)).xyz;                  \n\
    V = normalize( -P );                                \n\
\
    color = illuminate(); color.a = 1.0;                \n\
\
    gl_Position = u_worldViewProjectionMatrix * vec4(a_vertex3, 1.0);     \n\
\
    UV = a_txCoord;                                     \n\
}                                                       \n\
"
