#pragma once

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

#include "WinApp.h"

class CommandDirectX
{
private:
	//	コンストラクタ等、未定義
	CommandDirectX() = default;
	~CommandDirectX() = default;
private:
	WinApp* winApp_;
	int32_t bufferWidth_;
	int32_t bufferHeight_;
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// これにより1つしか生成されなくなる
	/// </summary>
	/// <returns></returns>
	static CommandDirectX* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	/// <param name="bufferWidth"></param>
	/// <param name="bufferHeight"></param>
	void Initialize(WinApp* winApp, int32_t bufferWidth, int32_t bufferHeight);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
private:

	/// <summary>
	/// DXGIファクトリーの生成
	/// </summary>
	/// <returns></returns>
	void CreateFactry();

	/// <summary>
	/// 使用するアダプタを決める
	/// </summary>
	/// <returns></returns>
	void SelectAdapter();

	/// <summary>
	/// デバイスの生成
	/// </summary>
	void CreateDevice();

	/// <summary>
	/// コマンドキューの生成
	/// </summary>
	void CreateCommandQueue();

	/// <summary>
	/// コマンドリストの生成
	/// </summary>
	void CreateCommandList();

	/// <summary>
	/// スワップチェインの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲットビューの生成
	/// </summary>
	void CreateRenderTargetView();

	/// <summary>
	/// フェンスの作成
	/// </summary>
	void CreateFence();

	/// <summary>
	/// 画面を指定色でクリアする
	/// </summary>
	void ClearRenderTarget();

	

private:
	//	DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory;
	//	使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter;
	//	デバイスの生成
	ID3D12Device* device;
	//	GPUに命令を投げるキューの生成
	ID3D12CommandQueue* commandQueue;
	//	命令保存用メモリ管理機構の生成
	ID3D12CommandAllocator* commandAllocator;
	//	GPUの命令群の生成
	ID3D12GraphicsCommandList* commandList;
	//	buffer切り替えのためのスワップチェインを生成
	IDXGISwapChain4* swapChain;
	//	ディスクリプタヒープの生成
	//	RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	ID3D12DescriptorHeap* rtvDescriptorHeap;
	//	SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	ID3D12DescriptorHeap* srvDescriptorHeap;
	//	SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2];
	//	フェンスの生成
	ID3D12Fence* fence;
	uint64_t fenceValue = 0;
	//	RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2] = {};


public:
	/// <summary>
	/// デバイスの取得
	/// </summary>
	ID3D12Device* GetDevice() const { return device; };

	ID3D12GraphicsCommandList* GetList() const { return commandList; };

};

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);