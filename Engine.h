#pragma once
#include "WinApp.h"
#include "CommandDirectX.h"
#include "ShaderManager.h"

class Engine
{

private:
	
public:

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

};
