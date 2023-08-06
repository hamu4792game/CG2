#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include <wrl.h>

//	GraphicsPipelineStateDesc
struct PSD {
	D3D12_ROOT_SIGNATURE_DESC sigDesc{};
	D3D12_DESCRIPTOR_RANGE range[2] = {};
	D3D12_ROOT_PARAMETER rootParameter[1] = {};
	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[3] = {};


};

class GraphicsPipeline
{
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

public:
	void CreateGraphicsPipeline();

	void CreateRootSignature();

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSig() { return rootSignature; };

};

