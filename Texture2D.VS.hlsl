#include "Texture2D.hlsli"

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD,float4 rgba : COLOR)
{
    VertexOutput output;
    output.potision = pos;
    output.texcoord = uv;
    output.color = rgba;
	return output;
}
