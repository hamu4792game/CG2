#include "Texture2D.hlsli"

Texture2D<float4> gDestTexture : register(t0);  // 基本色テクスチャ
Texture2D<float4> gScrTexture : register(t1);   // 合成色テクスチャ
SamplerState gSampler : register(s0);


cbuffer Monochrome : register(b0)
{
    float2 monochromePibot;
    float monochromeRate;
}

cbuffer Iro : register(b1)
{
    float4 color;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float4 dest = gDestTexture.Sample(gSampler, input.texcoord) * color;
    float4 src = gScrTexture.Sample(gSampler, input.texcoord) * color;
    
    //  乗算
    //  dest * src
    float4 result;
	result.rgb = dest.rgb * src.rgb;
	result.a = src.a * color.a;
    
    return result;
}