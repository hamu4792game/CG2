#include "Engine.h"

#include <Windows.h>
#include <cassert>

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include "Engine/Log.h"
#include "Engine/Texture/Texture2D.h"

WinApp* winApp = nullptr;
CommandDirectX* comDirect = nullptr;
ShaderManager* sManager = nullptr;

ID3D12Debug1* debugController = nullptr;
ID3D12InfoQueue* infoQueue = nullptr;

Engine* Engine::GetInstance()
{
	static Engine instance;
	return &instance;
}

void Engine::Initialize(const char* title, int width, int height)
{
	Engine::GetInstance()->WindowWidth = width;
	Engine::GetInstance()->WindowHeight = height;

	//	COMの初期化を行う
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	//	ゲームウィンドウの作成
	winApp = WinApp::GetInstance();
	winApp->CreateGameWindow(ConvertString(title).c_str(), Engine::GetInstance()->WindowWidth, Engine::GetInstance()->WindowHeight);

#ifdef _Debug
	//	デバッグレイヤー
	DebugLayer();
#endif
	//	ゲームウィンドウを表示する
	ShowWindow(winApp->GetHwnd(), SW_SHOW);

	//	DirectXの初期化
	comDirect = CommandDirectX::GetInstance();
	comDirect->Initialize(winApp, Engine::GetInstance()->WindowWidth, Engine::GetInstance()->WindowHeight);

	sManager = ShaderManager::GetInstance();
	sManager->DXcInitialize();
#ifdef _DEBUG
	//	ここまででエラーがあれば止める
	ErrorStop(comDirect->GetDevice());
#endif

}

void Engine::Finalize()
{
	sManager->Finalize();
	sManager = nullptr;
	comDirect->Finalize();
	comDirect = nullptr;
	winApp->DeleteGameWindow();
}

void Engine::BeginFrame()
{
	comDirect->PreDraw();
}

void Engine::EndFrame()
{
	comDirect->PostDraw();
}

int Engine::ProcessMessage()
{
	return winApp->ProcessMessage();
}

#ifdef _DEBUG

void Engine::DebugLayer()
{
	debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		//	デバッグレイヤーを有効可する
		debugController->EnableDebugLayer();
		//	さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(true);
	}
}

void Engine::ErrorStop(ID3D12Device* device_)
{
	infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		//	やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//	エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//	警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//	抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//	Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//	https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//	抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//	指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		//	解放
		infoQueue->Release();
	}
}

#endif


//	BufferResourceを作る関数
ID3D12Resource* Engine::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

	//	頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //	uploadHeapを使う
	//	頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//	バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; //	リソースのサイズ。今回はVector4を3頂点分
	//	バッファの場合はこれらは1にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//	バッファの頂点はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//	実際に頂点リソースを作る
	ID3D12Resource* Resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	hr = device->GetDeviceRemovedReason();
	assert(SUCCEEDED(hr));
	return Resource;

}

//	Resourceを生成してreturnする
ID3D12Resource* Engine::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	//	metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);	//	textureの幅
	resourceDesc.Height = UINT(metadata.height);	//	textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);	//	bitmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);	//	奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;	//	TextureのFormat
	resourceDesc.SampleDesc.Count = 1;	//	サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//	Textureの次元数。普段使っているのは二次元
	//	利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;	//	VRAM上に作成する
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	//	WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//	プロセッサの近くに配置
	//	Resourceの設定する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,	//	Heapの設定
		D3D12_HEAP_FLAG_NONE,	//	Heapの特殊な設定。特になし
		&resourceDesc,	//	Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	//	データ転送される設定
		nullptr,	//	Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));	//	作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

ID3D12Device* Engine::GetDevice() { return comDirect->GetDevice(); }

ID3D12GraphicsCommandList* Engine::GetList() { return comDirect->GetList(); }
