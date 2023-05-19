
struct TransformationMatrix {
	float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderOutPut {
	float32_t4 position : SV_POSITION;
};

struct VertexShaderInPut {
	float32_t4 position : POSITION0;
};

VertexShaderOutPut main(VertexShaderInPut input) {
	VertexShaderOutPut output;
	output.position = mul(input.position, gTransformationMatrix.WVP);
	return output;
}