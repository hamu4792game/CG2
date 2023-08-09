#pragma once
#include "Engine/Base/WinApp.h"
#include "Engine/Base/CommandDirectX.h"
#include "Engine/Manager/ShaderManager.h"

#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

class Engine
{	
public:

	static Engine* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="title">ウィンドウのタイトル</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	static void Initialize(const char* title, int width, int height);

	/// <summary>
	/// 解放処理
	/// </summary>
	static void Finalize();

	static void BeginFrame();

	static void EndFrame();

private:

	/// <summary>
	/// Windowsのメッセージを処理する
	/// </summary>
	/// <returns>1: ゲーム終了 0: ゲーム継続</returns>
	static int ProcessMessage();

#ifdef _DEBUG


	/// <summary>
	/// デバッグレイヤーの有効化
	/// </summary>
	static void DebugLayer();

	/// <summary>
	/// エラーや警告時に止める
	/// </summary>
	static void ErrorStop(ID3D12Device* device_);

#endif 

public:
	//	BufferResourceを作る関数
	static ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	//	Resourceを生成してreturnする
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

public:
	static ID3D12Device* GetDevice();
	static ID3D12GraphicsCommandList* GetList();

public:
	int WindowWidth;
	int WindowHeight;

};

class D3DResourceLeakChecker {
public:
	D3DResourceLeakChecker() = default;
	~D3DResourceLeakChecker() {
		//	リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};
