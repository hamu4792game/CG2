#include "Texture2D.h"
#include "Engine.h"
#include "CommandDirectX.h"
#include "ShaderManager.h"
#include "Log.h"


Texture2D::~Texture2D()
{
	graphicsPipelineState->Release();
	rootSignature->Release();
	vertexShader->Release();
	vertexResource->Release();
	SRVHeap->Release();
	pixelShader->Release();
	resource->Release();
}

void Texture2D::Finalize()
{
	graphicsPipelineState->Release();
	rootSignature->Release();
	vertexShader->Release();
	vertexResource->Release();
	SRVHeap->Release();
	pixelShader->Release();
	resource->Release();
}

void Texture2D::Texture(const std::string& filePath, const std::string& vsFileName, const std::string& psFileName)
{
	CreateDescriptor(filePath);
	CreateShader(vsFileName, psFileName);
	CreateGraphicsPipeline();
}

void Texture2D::CreateDescriptor(const std::string& filePath)
{
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	resource = Engine::CreateTextureResource(Engine::GetDevice(), metaData);
	UploadTextureData(resource, mipImages);

	//	デスクリプタヒープを生成
	SRVHeap = CreateDescriptorHeap(Engine::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);

	//	設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = metaData.format;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metaData.mipLevels);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//	
	Engine::GetDevice()->CreateShaderResourceView(resource, &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
}

void Texture2D::CreateShader(const std::string& vsFileName, const std::string& psFileName)
{
	vertexShader = ShaderManager::GetInstance()->CompileShader(ConvertString(vsFileName), L"vs_6_0");
	pixelShader = ShaderManager::GetInstance()->CompileShader(ConvertString(psFileName), L"ps_6_0");
}

void Texture2D::CreateGraphicsPipeline()
{
	//	頂点データ
	VertexData vertex[4] = {
		{{-1.0f,-1.0f,0.1f,1.0f},{0.0f,1.0f}},
		{{-1.0f,1.0f,0.1f,1.0f},{0.0f,0.0f}},
		{{1.0f,-1.0f,0.1f,1.0f},{1.0f,1.0f}},
		{{1.0f,1.0f,0.1f,1.0f},{1.0f,0.0f}},
	};
	vertexResource = Engine::CreateBufferResource(Engine::GetDevice(), sizeof(vertex));

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//	
	VertexData* mapData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	for (auto i = 0; i < _countof(vertex); i++)
	{
		mapData[i] = vertex[i];
	}
	vertexResource->Unmap(0, nullptr);

	//
	D3D12_ROOT_SIGNATURE_DESC sigDesc{};
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameter[1] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = range;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(range);

	sigDesc.pParameters = rootParameter;
	sigDesc.NumParameters = _countof(rootParameter);

	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSampler[0].ShaderRegister = 0;
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	sigDesc.pStaticSamplers = staticSampler;
	sigDesc.NumStaticSamplers = _countof(staticSampler);

	//	シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//	バイナリを元に生成
	hr = Engine::GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
	if (errorBlob)
	{
		errorBlob->Release();
	}
	signatureBlob->Release();

	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = inputElementDesc;
	layoutDesc.NumElements = _countof(inputElementDesc);

	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.DepthClipEnable = true;

	//	PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;	//	RootSignature
	graphicsPipelineStateDesc.InputLayout = layoutDesc;	//	InputLayout
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.VS = {
		vertexShader->GetBufferPointer(),
		vertexShader->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
		pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize()
	};
	
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	//	ここからBlendの設定
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;	//	基本的にAdd
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	//	実際に生成
	hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void Texture2D::Draw()
{
	Engine::GetList()->SetGraphicsRootSignature(rootSignature);
	Engine::GetList()->SetPipelineState(graphicsPipelineState);
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	Engine::GetList()->SetDescriptorHeaps(1, &SRVHeap);
	Engine::GetList()->SetGraphicsRootDescriptorTable(0, SRVHeap->GetGPUDescriptorHandleForHeapStart());
	Engine::GetList()->DrawInstanced(4, 1, 0, 0);
}


//	DirectX12のTextureResourceを作る
DirectX::ScratchImage Texture2D::LoadTexture(const std::string& filePath) {
	//	テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//	ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	//	ミニマップ付きのデータを返す
	return mipImages;
}

void Texture2D::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//	meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//	全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}

}
