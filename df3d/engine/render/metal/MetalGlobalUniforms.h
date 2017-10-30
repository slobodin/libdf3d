#pragma once

#include <simd/simd.h>

using namespace metal;

typedef struct
{
    simd::float4 position;
    simd::float4 color;
} MetalLightUniform;

typedef struct
{
    simd::float4x4 u_worldViewProjectionMatrix;
    simd::float4x4 u_worldViewMatrix;
    simd::float3x3 u_worldViewMatrix3x3;
    simd::float4x4 u_viewMatrixInverse;
    simd::float4x4 u_viewMatrix;
    simd::float4x4 u_projectionMatrix;
    simd::float4x4 u_worldMatrix;
    simd::float4x4 u_worldMatrixInverse;
    simd::float3x3 u_normalMatrix;

    simd::float4 u_globalAmbient;
    simd::float3 u_cameraPosition;
    simd::float1 u_fogDensity;
    simd::float4 u_fogColor;
    simd::float1 u_elapsedTime;

    MetalLightUniform light0;
    MetalLightUniform light1;
} MetalGlobalUniforms;
