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
    float w, h, levels;
    gTexture.GetDimensions(0, w, h, levels);
    float dx = 1.0f / w; float dy = 1.0f / h;
    
    
    float4 ret = float4(0.0f,0.0f,0.0f,0.0f);
    
    //  左下
    ret += gTexture.Sample(gSampler, input.texcoord + float2(-dx, -dy));
    //  左中
    ret += gTexture.Sample(gSampler, input.texcoord + float2(-dx, 0.0f));
    //  左上
    ret += gTexture.Sample(gSampler, input.texcoord + float2(-dx, dy));
    //  中下
    ret += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, -dy));
    //  中央
    ret += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, 0.0f));
    //　中上
    ret += gTexture.Sample(gSampler, input.texcoord + float2(0.0f, dy));
    //  右下
    ret += gTexture.Sample(gSampler, input.texcoord + float2(dx, -dy));
    //  右中
    ret += gTexture.Sample(gSampler, input.texcoord + float2(dx, 0.0f));
    //  右下
    ret += gTexture.Sample(gSampler, input.texcoord + float2(dx, dy));
    
    ret = ret / 9.0f;
    
    //  どれだけずらすか
    //float4 output = gTexture.Sample(gSampler, input.texcoord + float2(-100.f * dx, dy));
    
    //float2 posCenter = input.potision.xy - center;
    //clip(length(posCenter) - radius);
    
    return ret;
}