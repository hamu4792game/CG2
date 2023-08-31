#include "Texture2D.h"
#include "Engine/Engine.h"
#include "Engine/Base/CommandDirectX.h"
#include "Engine/Manager/ShaderManager.h"
#include "Engine/Manager/TextureManager.h"
#include "Engine/Log.h"

#include "externals/imgui/imgui.h"


void Texture2D::Finalize()
{
	graphicsPipelineState->Release();
	rootSignature->Release();
	indexResource->Release();
	vertexShader->Release();
	vertexResource->Release();
	SRVHeap->Release();
	pixelShader->Release();
	resource[0]->Release();
}

void Texture2D::Texture(const std::string& filePath, const std::string& vsFileName, const std::string& psFileName)
{
	cBuffer->pibot = { 0.0f,0.0f };
	cBuffer->rate = 1.0f;
	*cColor = { 1.0f,1.0f,1.0f,1.0f };
	*cMat = MakeIdentity4x4();
	blend = BlendMode::Normal;

	CreateDescriptor(filePath);
	CreateShader(vsFileName, psFileName);
	CreateVertexResource();
	CreateGraphicsPipeline();
}

void Texture2D::CreateDescriptor(const std::string& filePath)
{
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	resource[0] = Engine::CreateTextureResource(Engine::GetDevice(), metaData);
	UploadTextureData(resource[0], mipImages);

	//	画像サイズの代入
	textureWidth = static_cast<uint32_t>(metaData.width);
	textureHeight = static_cast<uint32_t>(metaData.height);

	//	デスクリプタヒープを生成
	SRVHeap = CreateDescriptorHeap(Engine::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);
	auto descriptorHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();
	descriptorHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cMat.CreateView(descriptorHandle);
	descriptorHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cColor.CreateView(descriptorHandle);
	descriptorHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cBuffer.CreateView(descriptorHandle);
	//	設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = metaData.format;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metaData.mipLevels);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//	
	Engine::GetDevice()->CreateShaderResourceView(resource[0], &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
}

void Texture2D::CreateShader(const std::string& vsFileName, const std::string& psFileName)
{
	vertexShader = ShaderManager::GetInstance()->CompileShader(ConvertString(vsFileName), L"vs_6_0");
	pixelShader = ShaderManager::GetInstance()->CompileShader(ConvertString(psFileName), L"ps_6_0");
}

void Texture2D::CreateVertexResource()
{
	//	頂点データ
	VertexData vertex[4] = {
		{{-0.5f,0.5f,0.1f,1.0f},{0.0f,0.0f}},
		{{0.5f,0.5f,0.1f,1.0f},{1.0f,0.0f}},
		{{0.5f,-0.5f,0.1f,1.0f},{1.0f,1.0f}},
		{{-0.5f,-0.5f,0.1f,1.0f},{0.0f,1.0f}},
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
	//	重要
	vertexResource->Unmap(0, nullptr);

	// index情報を作る
	// 1.indexの情報を送るためにmap用のuint16_t型の配列を作る(中身は頂点の組み合わせ、要素番号)
	uint16_t indices[6] = { 0,1,3,1,2,3 };
	// 2.Resourceを生成
	indexResource = Engine::CreateBufferResource(Engine::GetDevice(), sizeof(indices));
	// 3.indexBufferViewを生成
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	indexBufferView.SizeInBytes = sizeof(indices);

	// 4.GPUに送るためのMap用のuint16_tポインタを生成
	uint16_t* indexData = nullptr;

	// 5.resoureceのMap関数を呼んで関連付けさせる
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	// 6.4で作ったものに3のものをコピー
	for (auto i = 0; i < _countof(indices); i++)
	{
		indexData[i] = indices[i];
	}

	// 7.Unmapする
	indexResource->Unmap(0, nullptr);
}

void Texture2D::CreateGraphicsPipeline()
{
	if (graphicsPipelineState) {
		//graphicsPipelineState->Release();
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

#pragma endregion

#pragma region BlendState

		//	ブレンドモードの設定
		D3D12_BLEND_DESC blendDesc{};
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

		//	ブレンドモードの分岐

		switch (blend)
		{
		case None:
			blendDesc.RenderTarget[0].BlendEnable = false;
			break;
		case Normal:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case Add:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case Subtract:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case Multily:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case Screen:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case Dark:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_MIN;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case Light:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_MAX;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		default:
			break;
		}

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
#pragma endregion

		//	実際に生成
		hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));
	}
}

void Texture2D::Draw(Vector2 pos, Vector2 scale, float rotate, Matrix4x4 viewProjectionMat, uint32_t color)
{	
	/*ImGui::Begin("a");
	ImGui::DragFloat2("%0.2f", &cBuffer->pibot.x, 1.0f);
	ImGui::End();*/
	//	色の変更
	*cColor = ChangeColor(color);
	*cMat = MakeAffineMatrix(
		{ scale.x * static_cast<float>(textureWidth),scale.y * static_cast<float>(textureHeight),1.0f },
		{ 0.0f,0.0f,rotate },
		{ pos.x,pos.y,0.5f }
	) * viewProjectionMat;

	Engine::GetList()->SetGraphicsRootSignature(rootSignature);
	Engine::GetList()->SetPipelineState(graphicsPipelineState);
	// インデックスを使わずに四角形以上を書くときは
	// 個々の設定はD3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	// インデックスを使うときは D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// ここにIndexBufferViewを設定
	Engine::GetList()->IASetIndexBuffer(&indexBufferView);

	Engine::GetList()->SetDescriptorHeaps(1, &SRVHeap);
	Engine::GetList()->SetGraphicsRootDescriptorTable(0, SRVHeap->GetGPUDescriptorHandleForHeapStart());
	Engine::GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Texture2D::SetBlend(BlendMode blend_)
{
	blend = blend_;
	CreateGraphicsPipeline();
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

