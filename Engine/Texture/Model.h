#pragma once

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "math/Vector4.h"
#include "Engine/ConstantBuffer.h"
#include "math/Matrix4x4.h"
#include "Engine/Manager/TextureManager.h"
#include <wrl.h>

class Model
{
public:
	Model() = default;
	~Model() = default;

private:

	ModelData modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource[1] = {};
	ID3D12DescriptorHeap* SRVHeap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	//	depthStencilResourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;
	//	DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;

	struct Mono {
		Vector2 pibot;
		float rate;
	};
	ConstantBuffer<Matrix4x4> cMat;
	ConstantBuffer<Vector4> cColor;
	ConstantBuffer<Mono> cBuffer;

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

public:

	void Texture(const std::string& filePath, const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateDescriptor(const std::string& filePath);

	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateVertexResource();

	void CreateVertexSphere();

	void CreateGraphicsPipeline();

public:
	void Draw(Vector3 pos, Vector3 scale, Vector3 rotate, Matrix4x4 viewProjectionMat, uint32_t color);

};
