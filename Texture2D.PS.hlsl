#include "Texture2D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer Circle : register(b0)
{
    float2 center;
    float radius;
}

float4 main(VertexOutput input) : SV_TARGET
{
    float4 output = gTexture.Sample(gSampler, input.texcoord);
    
    float2 posCenter = input.potision.xy - center;
    clip(length(posCenter) - radius);
    return output;
}