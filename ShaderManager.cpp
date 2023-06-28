#include "ShaderManager.h"
#include <cassert>
#include <combaseapi.h>

#include <dxcapi.h>
#include "math/Vector4.h"
#pragma comment(lib,"dxcompiler.lib")


ShaderManager::~ShaderManager()
{
	dxcUtils->Release();
	dxcCompiler->Release();
	includeHandler->Release();
}

ShaderManager* ShaderManager::GetInstance()
{
	static ShaderManager instance;
	return &instance;
}

void ShaderManager::DXcInitialize()
{
	HRESULT hr = S_FALSE;
	//	dxcCompilerを初期化
	dxcUtils = nullptr;
	dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//	現時点でincludeはしないが、includeに対応するための設定を行っておく
	includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
}

void ShaderManager::Finalize()
{
	dxcUtils->Release();
	dxcCompiler->Release();
	includeHandler->Release();
}

/*
void ShaderManager::PSO(struct ID3D12Device* device)
{
	HRESULT hr = S_FALSE;

	//	RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//	RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//	CBVを使う b0のbと一致する
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//	PixelShaderで使う 
	rootParameters[0].Descriptor.ShaderRegister = 0;	//	レジスタ番号とバインド b0の0と一致する。もしb11と紐づけたいなら11となる

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//	CBVを使う b0のbと一致する
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//	PixelShaderで使う 
	rootParameters[1].Descriptor.ShaderRegister = 0;	//	レジスタ番号とバインド b0の0と一致する。もしb11と紐づけたいなら11となる

	//	DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	//	0から始まる
	descriptorRange->NumDescriptors = 1;	//	数は1つ
	descriptorRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//	SRVを使う
	descriptorRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	//	offsetを自動計算

	//	DescriptorTable
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//	DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//	PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;	//	Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//	Tableで利用する数

	descriptionRootSignature.pParameters = rootParameters;	//	ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	//	配列の長さ

	//	Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//	バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//	0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//	比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	//	ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;	//	レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//	PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//	シリアライズしてバイナリにする
	signatureBlob = nullptr;
	errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//	バイナリを元に生成
	rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	//	InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//	BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//	すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//	RasterizerStateの設定を行う
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//	裏面（時計回り）を表示する
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//	三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//	shaderをコンパイルする
	vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	//	DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//	Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//	書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//	比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//	PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;	//	RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;	//	InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() }; //	VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };  //	PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //	BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;	//	RasterizerState
	//	書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//	利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//	DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//	実際に生成
	graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

D3D12_VERTEX_BUFFER_VIEW ShaderManager::CreateVBV(struct ID3D12Device* device)
{
	vertexResource = CreateBufferResource(device, sizeof(VertexData) * 3);

	//	頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//	リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//	使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	//	1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//	頂点リソースにデータを書き込む
	vertexData = nullptr;
	//	書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//	左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//	上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//	右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	return vertexBufferView;
}

*/

//	shaderをcompileする関数
IDxcBlob* ShaderManager::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	//	1.hlslファイルを読む
	//	これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//	hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//	読めなかったら止める
	assert(SUCCEEDED(hr));
	//	読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;	//	utf8の文字コードであることを通知

	//	Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),	//コンパイル対象のhlslファイル名
		L"-E",L"main",		//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,		//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",	//デバッグ用の城尾法を埋め込む
		L"-Od",				//最適化を外しておく
		L"-Zpr",			//メモリレイアウトは行優先
	};

	//	p24
	//	実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	//	読み込んだファイル
		arguments,				//	コンパイルオプション
		_countof(arguments),	//	コンパイルオプションの数
		includeHandler,			//	includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	//	コンパイル結果
	);
	//	コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//	警告、エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//	警告、エラーダメ絶対
		assert(false);
	}

	//	コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//	成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//	もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//	実行用のバイナリを返却
	return shaderBlob;

}

////	BufferResourceを作る関数
//ID3D12Resource* ShaderManager::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
//
//	//	頂点リソース用のヒープの設定
//	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
//	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //	uploadHeapを使う
//	//	頂点リソースの設定
//	D3D12_RESOURCE_DESC ResourceDesc{};
//	//	バッファリソース。テクスチャの場合はまた別の設定をする
//	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	ResourceDesc.Width = sizeInBytes; //	リソースのサイズ。今回はVector4を3頂点分
//	//	バッファの場合はこれらは1にする決まり
//	ResourceDesc.Height = 1;
//	ResourceDesc.DepthOrArraySize = 1;
//	ResourceDesc.MipLevels = 1;
//	ResourceDesc.SampleDesc.Count = 1;
//	//	バッファの頂点はこれにする決まり
//	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//	//	実際に頂点リソースを作る
//	ID3D12Resource* Resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
//		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
//	assert(SUCCEEDED(hr));
//	return Resource;
//}
