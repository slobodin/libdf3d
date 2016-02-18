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
attribute vec3 a_vertex3;                    \n\
attribute vec3 a_normal;                    \n\
attribute vec2 a_txCoord;                   \n\
\
uniform mat4 u_worldViewMatrix;               \n\
uniform mat4 u_worldViewProjectionMatrix;     \n\
uniform mat3 u_normalMatrix;                  \n\
\
uniform Material material;                  \n\
uniform vec4 u_globalAmbient;                 \n\
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
    // Clamping just for sanity check (can't be more than 1.0) \n\
    float lambertDiffuse = clamp(dot(N, -L), 0.0, 1.0);     \n\
    vec3 diffuse = material.diffuse.rgb * current_light.diffuse * lambertDiffuse;\n\
\
    // Compute specular light. NOTE: Phong model.           \n\
    vec3 r = reflect( L, N );                               \n\
    float phongSpecular = pow(clamp(dot(r, V), 0.0, 1.0), material.shininess); \n\
    vec3 specular = material.specular.rgb * current_light.specular * phongSpecular; \n\
\
    color.rgb += diffuse + specular;                        \n\
}                                                           \n\
\
void main()                                             \n\
{                                                       \n\
    N = normalize( u_normalMatrix * a_normal );           \n\
    P = (u_worldViewMatrix * vec4(a_vertex3, 1.0)).xyz;                  \n\
    V = normalize( -P );                                \n\
\
    color = vec4(0.0, 0.0, 0.0, 1.0);                   \n\
\
    illuminate();                                       \n\
\
    gl_Position = u_worldViewProjectionMatrix * vec4(a_vertex3, 1.0);     \n\
\
    UV = a_txCoord;                                     \n\
}                                                       \n\
"
