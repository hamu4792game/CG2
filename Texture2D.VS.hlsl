#include "Texture2D.hlsli"

VertexOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VertexOutput output;
    output.potision = pos;
    output.texcoord = uv;
	return output;
}
