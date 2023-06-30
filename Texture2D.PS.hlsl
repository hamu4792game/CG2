#include "Texture2D.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


cbuffer Circle : register(b0)
{
    float2 center;
    float radius;
}

cbuffer Iro : register(b1)
{
    float4 color;
};

float4 main(VertexOutput input) : SV_TARGET
{
    VertexOutput output = gTexture.Sample(gSampler, input.texcoord);
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = input.color * textureColor;
    
    //float2 posCenter = input.potision.xy - center;
    //clip(length(posCenter) - radius);
    
    return output;
}