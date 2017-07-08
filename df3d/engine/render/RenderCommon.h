#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

extern const uint32_t TEXTURE_FILTERING_NEAREST;
extern const uint32_t TEXTURE_FILTERING_BILINEAR;
extern const uint32_t TEXTURE_FILTERING_TRILINEAR;
extern const uint32_t TEXTURE_FILTERING_ANISOTROPIC;
extern const uint32_t TEXTURE_FILTERING_MASK;

extern const uint32_t TEXTURE_WRAP_MODE_REPEAT;
extern const uint32_t TEXTURE_WRAP_MODE_CLAMP;
extern const uint32_t TEXTURE_WRAP_MODE_MASK;

enum class PixelFormat
{
    INVALID,

    RGB,
    RGBA,
    DEPTH,

    KTX
};

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

DF3D_DECLARE_HANDLE(VertexBufferHandle)
DF3D_DECLARE_HANDLE(IndexBufferHandle)
DF3D_DECLARE_HANDLE(TextureHandle)
DF3D_DECLARE_HANDLE(ShaderHandle)
DF3D_DECLARE_HANDLE(GpuProgramHandle)
DF3D_DECLARE_HANDLE(UniformHandle)
DF3D_DECLARE_HANDLE(FrameBufferHandle)

struct FrameStats
{
    size_t drawCalls = 0;
    size_t totalTriangles = 0;
    size_t totalLines = 0;

    size_t textures = 0;
    size_t gpuMemBytes = 0;
};

#define LIGHTS_MAX 2

enum class SharedUniformType
{
    WORLD_VIEW_PROJECTION_MATRIX_UNIFORM,
    WORLD_VIEW_MATRIX_UNIFORM,
    WORLD_VIEW_3X3_MATRIX_UNIFORM,
    VIEW_INVERSE_MATRIX_UNIFORM,
    VIEW_MATRIX_UNIFORM,
    WORLD_INVERSE_MATRIX_UNIFORM,
    WORLD_MATRIX_UNIFORM,
    NORMAL_MATRIX_UNIFORM,
    PROJECTION_MATRIX_UNIFORM,

    CAMERA_POSITION_UNIFORM,

    GLOBAL_AMBIENT_UNIFORM,

    FOG_DENSITY_UNIFORM,
    FOG_COLOR_UNIFORM,

    PIXEL_SIZE_UNIFORM,

    ELAPSED_TIME_UNIFORM,

    // FIXME: forward rendering (no multi-pass) for 2 lights only.
    // TODO: generate shaders or deferred (OpenGL ES 3.0).
    SCENE_LIGHT_0_COLOR_UNIFORM,
    SCENE_LIGHT_0_POSITION_UNIFORM,
    SCENE_LIGHT_1_COLOR_UNIFORM,
    SCENE_LIGHT_1_POSITION_UNIFORM,

    COUNT
};

}
