#include "Engine.h"

#include <Windows.h>
#include <cassert>

#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")


//	imguiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
//	関数の外部宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include "Log.h"

WinApp* winApp = nullptr;
CommandDirectX* comDirect = nullptr;
ShaderManager* sManager = nullptr;

ID3D12Debug1* debugController;
ID3D12InfoQueue* infoQueue;

void Engine::Initialize(const char* title, int width, int height)
{
	//	COMの初期化を行う
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//	ゲームウィンドウの作成
	winApp = WinApp::GetInstance();
	winApp->CreateGameWindow(ConvertString(title).c_str(), width, height);

	//	デバッグレイヤー
	DebugLayer();

	//	ゲームウィンドウを表示する
	ShowWindow(winApp->GetHwnd(), SW_SHOW);

	//	DirectXの初期化
	comDirect = CommandDirectX::GetInstance();
	comDirect->Initialize(winApp, width, height);

	sManager = ShaderManager::GetInstance();
	sManager->DXcInitialize();
	//	ここまででエラーがあれば止める
	ErrorStop(comDirect->GetDevice());

}

void Engine::Finalize()
{
	comDirect->Finalize();
	sManager->Finalize();
	debugController->Release();
	infoQueue->Release();
	//	リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
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