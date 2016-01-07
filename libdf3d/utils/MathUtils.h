#pragma once

namespace df3d { namespace utils { namespace math {

extern const DF3D_DLL glm::vec4 XAxis;
extern const DF3D_DLL glm::vec4 YAxis;
extern const DF3D_DLL glm::vec4 ZAxis;
extern const DF3D_DLL glm::vec3 UnitVec3;

// Creates rotation which rotates from v1 to v2.
DF3D_DLL glm::quat fromToRotation(const glm::vec3 &v1, const glm::vec3 &v2);
DF3D_DLL glm::vec2 toPolar(const glm::vec2 &cartesian);
DF3D_DLL glm::vec2 fromPolar(float angle, float dist);
DF3D_DLL float signedDistanceToPlane(const glm::vec4 &plane, const glm::vec3 &point);
DF3D_DLL glm::vec3 safeNormalize(const glm::vec3 &v);

struct DF3D_DLL spherical
{
    float r;
    float yaw;
    float pitch;

    spherical(float r, float yaw, float pitch);
    spherical(const glm::vec3 &v);
    ~spherical();

    glm::vec3 toCartesian();
};

DF3D_DLL float gaussian(float x, float mean, float stddev);

} } }
