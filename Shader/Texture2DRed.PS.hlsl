#include "Texture2D.hlsli"

Texture2D<float4> gTexture : register(t0);
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
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord) * color;
    //  YUBの色を取り、輝度値を取り出す。(0.299f,0.587f,0.114f)これでYの値を取得
    //float Y = dot(textureColor.xyz, float3(0.299f, 0.587f, 0.114f));
    //  従来のグレースケール計算
    float Y = (textureColor.x + textureColor.y + textureColor.z) / 3.0f;
    
    //float red = clamp(textureColor.x,Y,textureColor.x);
    float red = max(textureColor.x,Y);
    /*ここで引いている値が残す色の基準値になる*/
    red = textureColor.x - 0.5f;
    //
    red = (red + abs(red)) * 0.5f;
    red = ceil(red);

    float3 monochrome = { Y, Y, Y };

    float len = monochromePibot.x - input.potision.x;
    len = clamp(len, 0.0f, 1.0f);

    textureColor.xyz = lerp(textureColor.xyz, monochrome, len - red);
    //textureColor.xyz = lerp(monochrome,textureColor.xyz, red);
    
    //float2 posCenter = input.potision.xy - center;
    //clip(length(posCenter) - radius);
    
    return textureColor;
}