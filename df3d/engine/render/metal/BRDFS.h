#pragma once

struct VertexInputNormalMap
{
    float3 a_vertex3 [[ attribute(0) ]];
    float2 a_txCoord [[ attribute(1) ]];
    float3 a_normal [[ attribute(3) ]];
    float3 a_tangent [[ attribute(4) ]];
    float3 a_bitangent [[ attribute(5) ]];
};

struct VertexOutputNormalMap
{
    float4 pos [[ position ]];
    float2 UV;

    half3 Viewer;
    half3 LightDir_1;
    half3 LightDir_2;

    float fogFactor;
};

// Lambert diffuse.
float4 lambertDiffuse(float3 normal, float3 light, float4 materialDiffuse, float4 lightDiffuse);


half4 BRDF_2(half3 normal, half3 light, half3 viewer, half4 albedo, half4 specular, half roughness);

float4 CalcSimpleLighting(constant MetalGlobalUniforms &globalUniforms,
                          float3 a_vertex3, float3 a_normal);
