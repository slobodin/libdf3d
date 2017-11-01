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

    float3 V;
    float3 L_1;
    float3 L_2;

    float fogFactor;
};

// Lambert diffuse.
float4 lambertDiffuse(float3 normal, float3 light, float4 materialDiffuse, float4 lightDiffuse);


float4 BRDF_2(float3 normal, float3 light, float3 viewer, float4 albedo, float4 specular, float roughness);

float4 CalcSimpleLighting(constant MetalGlobalUniforms &globalUniforms,
                          float3 a_vertex3, float3 a_normal);
