
struct VertexShaderOutPut {
	float32_t4 position : SV_POSITION;
};

struct VertexShaderInPut {
	float32_t4 position : POSITION0;
};

VertexShaderOutPut main(VertexShaderInPut input) {
	VertexShaderOutPut output;
	output.position = input.position;
	return output;
}