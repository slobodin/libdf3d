#pragma once

#include <simd/simd.h>

enum MetalTextureInputIndex
{
    TEXTURE_IDX_DIFFUSE_MAP = 0,
    TEXTURE_IDX_NORMAL_MAP,
    TEXTURE_IDX_EMISSIVE_MAP,
    TEXTURE_IDX_NOISE_MAP,

    TEXTURE_IDX_COUNT
};

typedef struct
{
    simd::float4 material_diffuse;
    simd::float4 material_specular;
    float material_shininess;
    float u_speed;
    float u_force;
    float u_xTextureScale;
    float u_time;
    float u_rimMinValue;
    int samplerIdx[TEXTURE_IDX_COUNT];
} MetalUserUniforms;

typedef struct
{
    simd::float4 position;
    simd::float4 color;
} MetalLight;

typedef struct
{
    simd::float4x4 u_worldViewProjectionMatrix;
    simd::float4x4 u_worldViewMatrix;
    simd::float4x4 u_viewMatrixInverse;
    simd::float4x4 u_viewMatrix;
    simd::float4x4 u_projectionMatrix;
    simd::float4x4 u_worldMatrix;
    simd::float4x4 u_worldMatrixInverse;
    simd::float3x3 u_normalMatrix;
    simd::float3x3 u_worldViewMatrix3x3;

    MetalUserUniforms userUniforms;

    MetalLight light0;
    MetalLight light1;

    simd::float4 u_globalAmbient;
    simd::float4 u_fogColor;

    simd::float3 u_cameraPosition;
    float u_fogDensity;
    float u_elapsedTime;
} MetalGlobalUniforms;
