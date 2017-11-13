#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

const uint32_t TEXTURE_FILTERING_NEAREST        = 0x1;
const uint32_t TEXTURE_FILTERING_BILINEAR       = 0x2;
const uint32_t TEXTURE_FILTERING_TRILINEAR      = 0x3;
const uint32_t TEXTURE_FILTERING_ANISOTROPIC    = 0x4;
const uint32_t TEXTURE_FILTERING_MASK           = 0x7;

const uint32_t TEXTURE_WRAP_MODE_REPEAT         = 0x08;
const uint32_t TEXTURE_WRAP_MODE_CLAMP          = 0x10;
const uint32_t TEXTURE_WRAP_MODE_MASK           = 0x18;

// Depth test states. Depth test is disabled by default.
//! Never passes.
const uint64_t RENDER_STATE_DEPTH_NEVER     = 0x1;
//! Passes if the incoming depth value is less than the stored depth value.
const uint64_t RENDER_STATE_DEPTH_LESS      = 0x2;
//! Passes if the incoming depth value is equal to the stored depth value.
const uint64_t RENDER_STATE_DEPTH_EQUAL     = 0x3;
//! Passes if the incoming depth value is less than or equal to the stored depth value.
const uint64_t RENDER_STATE_DEPTH_LEQUAL    = 0x4;
//! Passes if the incoming depth value is greater than the stored depth value.
const uint64_t RENDER_STATE_DEPTH_GREATER   = 0x5;
//! Passes if the incoming depth value is not equal to the stored depth value.
const uint64_t RENDER_STATE_DEPTH_NOTEQUAL  = 0x6;
//! Passes if the incoming depth value is greater than or equal to the stored depth value.
const uint64_t RENDER_STATE_DEPTH_GEQUAL    = 0x7;
//! Always passes.
const uint64_t RENDER_STATE_DEPTH_ALWAYS    = 0x8;
const uint64_t RENDER_STATE_DEPTH_MASK      = 0xF;

// Face culling. By default is disabled.
// ! Culling enabled. Front face is CW.
const uint64_t RENDER_STATE_FRONT_FACE_CW   = 0x10;
// ! Culling enabled. Front face is CCW.
const uint64_t RENDER_STATE_FRONT_FACE_CCW  = 0x20;
const uint64_t RENDER_STATE_FACE_CULL_MASK  = 0x30;

// Misc states.
//! Enable depth write.
const uint64_t RENDER_STATE_DEPTH_WRITE         = 0x40;
const uint64_t RENDER_STATE_DEPTH_WRITE_MASK    = 0x40;

// Blending.
const uint64_t RENDER_STATE_BLEND_SRC_SHIFT = 0x8;
const uint64_t RENDER_STATE_BLEND_DST_SHIFT = 0xC;
const uint64_t RENDER_STATE_BLENDING_MASK   = 0xFF00;

const uint64_t RENDER_STATE_BLEND_ONE                   = 0x1;
const uint64_t RENDER_STATE_BLEND_SRC_ALPHA             = 0x2;
const uint64_t RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA   = 0x3;
const uint64_t RENDER_STATE_BLEND_FUNCTION_MASK         = 0xF;

DF3D_FINLINE uint64_t GetBlendingSrcFactor(uint64_t renderState)
{
    return (renderState >> RENDER_STATE_BLEND_SRC_SHIFT) & RENDER_STATE_BLEND_FUNCTION_MASK;
}

DF3D_FINLINE uint64_t GetBlendingDstFactor(uint64_t renderState)
{
    return (renderState >> RENDER_STATE_BLEND_DST_SHIFT) & RENDER_STATE_BLEND_FUNCTION_MASK;
}

DF3D_FINLINE uint64_t MakeBlendFunc(uint64_t srcFactor, uint64_t dstFactor)
{
    return (srcFactor << RENDER_STATE_BLEND_SRC_SHIFT) | (dstFactor << RENDER_STATE_BLEND_DST_SHIFT);
}

const uint64_t BLENDING_ALPHA = MakeBlendFunc(RENDER_STATE_BLEND_SRC_ALPHA, RENDER_STATE_BLEND_ONE_MINUS_SRC_ALPHA);
const uint64_t BLENDING_ADDALPHA = MakeBlendFunc(RENDER_STATE_BLEND_SRC_ALPHA, RENDER_STATE_BLEND_ONE);
const uint64_t BLENDING_ADD = MakeBlendFunc(RENDER_STATE_BLEND_ONE, RENDER_STATE_BLEND_ONE);

enum RenderQueueBucket
{
    RQ_BUCKET_LIT,
    RQ_BUCKET_NOT_LIT,
    RQ_BUCKET_TRANSPARENT,
    RQ_BUCKET_2D,
    RQ_BUCKET_DEBUG,

    RQ_BUCKET_COUNT
};

enum class Blending
{
    NONE,
    ADD,
    ALPHA,
    ADDALPHA
};

enum class PixelFormat
{
    INVALID,

    RGB,
    RGBA,
    DEPTH,

    KTX
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

enum class Topology : uint32_t
{
    LINES,
    TRIANGLES,
    LINE_STRIP,
    TRIANGLE_STRIP
};

enum IndicesType
{
    INDICES_16_BIT,
    INDICES_32_BIT
};

DF3D_DECLARE_HANDLE(VertexBufferHandle)
DF3D_DECLARE_HANDLE(IndexBufferHandle)
DF3D_DECLARE_HANDLE(TextureHandle)
DF3D_DECLARE_HANDLE(GPUProgramHandle)
DF3D_DECLARE_HANDLE(UniformHandle)

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

enum class RenderBackendID
{
    GL,
    METAL
};

struct Viewport
{
    int originX;
    int originY;
    int width;
    int height;

    bool operator== (const Viewport &other) const
    {
        return originX == other.originX && 
            originY == other.originY && 
            width == other.width && 
            height == other.height;
    }

    bool operator!= (const Viewport &other) const
    {
        return !(*this == other);
    }
};


}
