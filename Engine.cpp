#include "Engine.h"

#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <cassert>

#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include "math/Vector2.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"

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

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/DirectXTex/d3dx12.h"
#include <vector>


#pragma region mesh

//	Log関数
void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

//	stringからwstringへ変換
std::wstring ConvertString(const std::string& str) {
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
std::string ConvertString(const std::wstring& str) {
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

//	ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//	メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//	ウィンドウが破棄された
	case WM_DESTROY:
		//	OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//	標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//	shaderをcompileする関数
IDxcBlob* CompileShader(
	//	compilerするshaderファイルへのパス
	const std::wstring& filePath,
	//	compilerに使用するProfile
	const wchar_t* profile,
	//	初期化で生成したものを3つ	
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
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

//	BufferResourceを作る関数
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

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

//	DescriptorHeap作成の関数
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//	ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

//	DirectX12のTextureResourceを作る
DirectX::ScratchImage LoadTexture(const std::string& filePath) {
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

//	Resourceを生成してreturnする
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
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
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//	VRAM上に作成する
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	//	WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//	プロセッサの近くに配置
	//	Resourceの設定する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,	//	Heapの設定
		D3D12_HEAP_FLAG_NONE,	//	Heapの特殊な設定。特になし
		&resourceDesc,	//	Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	//	データ転送される設定
		nullptr,	//	Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));	//	作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

//	TextureResourceにデータを転送する
[[nodiscard]]	//戻り値を破棄してはならない
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
	//	中間リソース
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	//	データ転送をコマンドに積む
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//	Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

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





void Engine::Initialize(const char* title, int width, int height)
{
	//	COMの初期化を行う
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WNDCLASS wc{};
	//	ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//	ウィンドウクラス名（なんでも良い）
	wc.lpszClassName = L"CG2WindowClass";
	//	インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//	カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//	ウィンドウクラスを登録する
	RegisterClass(&wc);

	//	クライアント領域のサイズ
	const int32_t kClientWidth = width;
	const int32_t kClientHeight = height;

	//	ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//	クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,		//	利用するクラス名
		L"title",				//	タイトルバーの文字（何でもいい）
		WS_OVERLAPPEDWINDOW,	//	よく見るウィンドウスタイル
		CW_USEDEFAULT,			//	表示X座標（Windowsに任せる）
		CW_USEDEFAULT,			//	表示Y座標（WindowsOSに任せる）
		wrc.right - wrc.left,	//	ウィンドウ横幅	
		wrc.bottom - wrc.top,	//	ウィンドウ縦幅
		nullptr,				//	親ウィンドウハンドル
		nullptr,				//	メニューハンドル
		wc.hInstance,			//	インスタンスハンドル
		nullptr					//	オプション
	);

	//	デバッグレイヤー
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		//	デバッグレイヤーを有効可する
		debugController->EnableDebugLayer();
		//	さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(true);

	}

#endif 

	//	ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);


}
