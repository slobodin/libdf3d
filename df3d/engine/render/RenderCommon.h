#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

extern const uint32_t TEXTURE_FILTERING_NEAREST;
extern const uint32_t TEXTURE_FILTERING_BILINEAR;
extern const uint32_t TEXTURE_FILTERING_TRILINEAR;
extern const uint32_t TEXTURE_FILTERING_MASK;

extern const uint32_t TEXTURE_WRAP_MODE_REPEAT;
extern const uint32_t TEXTURE_WRAP_MODE_CLAMP;
extern const uint32_t TEXTURE_WRAP_MODE_MASK;

extern const uint32_t TEXTURE_MAX_ANISOTROPY;
extern const uint32_t TEXTURE_MAX_ANISOTROPY_MASK;

//! Hint to graphics backend as to how a buffer's data will be accessed.
enum class GpuBufferUsageType
{
    //! The data store contents will be modified once and used many times.
    STATIC,
    //! The data store contents will be modified repeatedly and used many times.
    DYNAMIC,
    //! The data store contents will be modified once and used at most a few times.
    STREAM
};

enum class CubeFace
{
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,

    COUNT
};

enum class ShaderType
{
    VERTEX,
    FRAGMENT,

    UNDEFINED
};

enum class Topology
{
    LINES,
    TRIANGLES,
    LINE_STRIP,
    TRIANGLE_STRIP
};

// TODO: refactor blending mode.
enum class BlendingMode : int
{
    NONE,
    ADDALPHA,
    ALPHA,
    ADD
};

enum class FaceCullMode : int
{
    NONE,
    FRONT,
    BACK
};

enum IndicesType
{
    INDICES_16_BIT,
    INDICES_32_BIT
};

DF3D_MAKE_HANDLE(VertexBufferHandle)
DF3D_MAKE_HANDLE(IndexBufferHandle)
DF3D_MAKE_HANDLE(TextureHandle)
DF3D_MAKE_HANDLE(ShaderHandle)
DF3D_MAKE_HANDLE(GpuProgramHandle)
DF3D_MAKE_HANDLE(UniformHandle)
DF3D_MAKE_HANDLE(FrameBufferHandle)

struct DF3D_DLL FrameStats
{
    size_t drawCalls = 0;
    size_t totalTriangles = 0;
    size_t totalLines = 0;

    size_t textures = 0;
    size_t gpuMemBytes = 0;
};

}
