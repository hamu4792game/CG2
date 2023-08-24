#include "Model.h"
#include "Engine/Manager/TextureManager.h"
#include <numbers>

#include "externals/imgui/imgui.h"

//	便利なtmpみたいなやつ
decltype(Model::rootSignature) Model::rootSignature;
decltype(Model::graphicsPipelineState) Model::graphicsPipelineState;

void Model::Texture(const std::string& filePath, const std::string& vsFileName, const std::string& psFileName)
{
	cBuffer->pibot = { 0.0f,0.0f };
	cBuffer->rate = 1.0f;
	*cColor = { 1.0f,1.0f,1.0f,1.0f };
	*cMat = MakeIdentity4x4();

	CreateDescriptor(filePath);
	CreateShader(vsFileName, psFileName);
	CreateVertexResource();
	CreateGraphicsPipeline();
}

void Model::CreateDescriptor(const std::string& filePath)
{
	//	モデル読み込み
	modelData = TextureManager::LoadObjFile(filePath);

	DirectX::ScratchImage mipImages = TextureManager::LoadTexture("./Resources/" + modelData.material.textureFilePath);
	//DirectX::ScratchImage mipImages = TextureManager::LoadTexture("./Resources/uvChecker.png");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	resource[0] = Engine::CreateTextureResource(Engine::GetDevice(), metaData);
	TextureManager::UploadTextureData(resource[0].Get(), mipImages);

	// 幅
	const UINT SRVsize = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//	デスクリプタヒープを生成
	SRVHeap = CreateDescriptorHeap(Engine::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);
	cMat.CreateView(GetCPUDescriptorHandle(SRVHeap, SRVsize, 1));
	cColor.CreateView(GetCPUDescriptorHandle(SRVHeap, SRVsize, 2));
	cBuffer.CreateView(GetCPUDescriptorHandle(SRVHeap, SRVsize, 3));

	//	設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = metaData.format;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metaData.mipLevels);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	
	Engine::GetDevice()->CreateShaderResourceView(resource[0].Get(), &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
	textureSrvHandleGPU = GetGPUDescriptorHandle(SRVHeap, Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), 0);
}

void Model::CreateShader(const std::string& vsFileName, const std::string& psFileName)
{
	vertexShader = ShaderManager::GetInstance()->CompileShader(ConvertString(vsFileName), L"vs_6_0");
	pixelShader = ShaderManager::GetInstance()->CompileShader(ConvertString(psFileName), L"ps_6_0");
}

void Model::CreateVertexResource()
{
	//	頂点データ
	
	vertexResource = Engine::CreateBufferResource(Engine::GetDevice(), sizeof(VertexData) * modelData.vertices.size());

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//	
	VertexData* mapData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	std::copy(modelData.vertices.begin(), modelData.vertices.end(), mapData);

	//	重要
	vertexResource->Unmap(0, nullptr);
}

void Model::CreateGraphicsPipeline()
{
	if (graphicsPipelineState) {
		return;
	}
	else {
		graphicsPipelineState = nullptr;

#pragma region RootSignature

		//	ルートシグネチャーの作成
		D3D12_ROOT_SIGNATURE_DESC sigDesc{};
		sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		D3D12_DESCRIPTOR_RANGE range[2] = {};
		range[0].BaseShaderRegister = 0;
		range[0].NumDescriptors = 1;	//	必要な数
		range[0].RegisterSpace = 0;
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		range[1].BaseShaderRegister = 0;
		range[1].NumDescriptors = 3;	//	必要な数
		range[1].RegisterSpace = 0;
		range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER rootParameter[1] = {};
		rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
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
		if (rootSignature)
		{
			rootSignature->Release();
		}
		rootSignature = nullptr;
		hr = Engine::GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));
		if (errorBlob)
		{
			errorBlob->Release();
		}
		signatureBlob->Release();

#pragma endregion

#pragma region InputLayout

		D3D12_INPUT_ELEMENT_DESC inputElementDesc[3] = {};
		inputElementDesc[0].SemanticName = "POSITION";
		inputElementDesc[0].SemanticIndex = 0;
		inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDesc[1].SemanticName = "TEXCOORD";
		inputElementDesc[1].SemanticIndex = 0;
		inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDesc[2].SemanticName = "NORMAL";
		inputElementDesc[2].SemanticIndex = 0;
		inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC layoutDesc{};
		layoutDesc.pInputElementDescs = inputElementDesc;
		layoutDesc.NumElements = _countof(inputElementDesc);

#pragma endregion

#pragma region BlendState

		//	ブレンドモードの設定
		D3D12_BLEND_DESC blendDesc{};
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

#pragma endregion

#pragma region RasterizerState

		//	ラスタライザの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//	裏面を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//	塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.DepthClipEnable = true;

#pragma endregion

#pragma region PSOの生成
		//	PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();	//	RootSignature
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

		//	DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		//	Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//	書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//	比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		//	DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

#pragma endregion

		//	実際に生成
		hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));
	}
}

void Model::Draw(WorldTransform worldTransform, const Matrix4x4& viewProjectionMat, uint32_t color)
{
	//	色の変更
	*cColor = ChangeColor(color);
	*cMat = worldTransform.worldMatrix * viewProjectionMat;

	Engine::GetList()->SetGraphicsRootSignature(rootSignature.Get());
	Engine::GetList()->SetPipelineState(graphicsPipelineState.Get());
	// インデックスを使わずに四角形以上を書くときは
	// 個々の設定はD3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	// インデックスを使うときは D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	
	Engine::GetList()->SetDescriptorHeaps(1, &SRVHeap);
	Engine::GetList()->SetGraphicsRootDescriptorTable(0, textureSrvHandleGPU);
	Engine::GetList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}