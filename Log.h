#pragma once

#include <Windows.h>
#include <string>
#include <format>

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <cassert>

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

//	Log関数
static void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

//	stringからwstringへ変換
static std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

//	wstringからstringへ変換
static std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

/*

#pragma region mesh

//	DepthStencilTextureを作る
ID3D12Resource* CreateDepthStencilTexture(ID3D12Device* device, int32_t width, int32_t height) {
	//	生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;	//	textureの幅
	resourceDesc.Height = height;	//	textureの高さ
	resourceDesc.MipLevels = 1;	//	bitmapの数
	resourceDesc.DepthOrArraySize = 1;	//	奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;;	//	DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;	//	サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//	二次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//	DepthStencilとして使う通知

	//	利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//	VRAM上に作る

	//	深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//	1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;;	//	フォーマット。Resourceと合わせる

	//	Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,	//	Heapの設定
		D3D12_HEAP_FLAG_NONE,	//	Heapの特殊な設定。特になし
		&resourceDesc,	//	Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//	深度値を書き込む状態にしておく
		&depthClearValue,	//	Clear最適値
		IID_PPV_ARGS(&resource));	//	作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

#pragma endregion

*/