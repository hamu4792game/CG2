#include <Windows.h>
#include "Engine.h"
#include "Log.h"

/*
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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("Engine", windowWidth, windowHeight);

	HRESULT hr = false;



	//	depthStencilResourceの生成
	ID3D12Resource* depthStencilResource = CreateDepthStencilTexture(device, windowWidth, windowHeight);
	//	2_5_p16
	//	DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//	DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;;	//	Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//	2DTexture
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	//	DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//	vertexResourceの作成
	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);

	//	マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));
	//	マテリアルにデータを書き込む
	Vector4* materialData = nullptr;
	//	書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//	通常で白
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	//	WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	//	データを書き込む
	Matrix4x4* wvpData = nullptr;
	//	書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//	単位行列を書き込んでおく
	*wvpData = MakeIdentity4x4();


	//	頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//	リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//	使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//	1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//	頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
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

	//	左下2
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	//	上2
	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	//	右下2
	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

	//	Sptite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

	//	頂点バッファビューを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//	リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//	使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//	1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//	頂点データの設定
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//	1枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };	//	左下
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };	//	左上
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };	//	右下
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//	2枚目の三角形
	vertexDataSprite[3].position = { 0.0f,0.0f,0.0f,1.0f };	//	左上
	vertexDataSprite[3].texcoord = { 0.0f,0.0f };
	vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };	//	右上
	vertexDataSprite[4].texcoord = { 1.0f,0.0f };
	vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };	//	右下
	vertexDataSprite[5].texcoord = { 1.0f,1.0f };

	//	Transform周りを作る
	//	Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
	//	データを書き込む
	Matrix4x4* transformationMatrixDataSprite = nullptr;

	//	書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//	単位行列を書き込んでおく
	*transformationMatrixDataSprite = MakeIdentity4x4();

	//	ビューポート
	D3D12_VIEWPORT viewport{};
	//	クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = windowWidth;
	viewport.Height = windowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//	シザー矩形
	D3D12_RECT scissorRect{};
	//	基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = windowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = windowHeight;

	//	ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//	Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, device, commandList);

	//	ShaderResourceViewを作る
	//	metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//	2Dtexture
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//	SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//	先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//	SRVの生成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

	//	transform変数の作成
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	Transform transform{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	Transform cameraTransform{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,-5.0f},
	};
	Transform transformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	
	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage())
	{
		//	ImGuiにframeの始まりを伝える
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//	ゲームの処理
		ImGui::ShowDemoWindow();
		transform.rotate.y += 0.03f;

		//	wvpMatrixの生成と設定
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = (worldMatrix * (viewMatrix * projectionMatrix));
		*wvpData = worldViewProjectionMatrix;

		//	Sprite用のWorldProjectionMatrixを作る
		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(windowWidth), float(windowHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * viewMatrixSprite * projectionMatrixSprite;
		*transformationMatrixDataSprite = worldViewProjectionMatrixSprite;


		//	ImGuiの内部コマンドを生成
		ImGui::Render();

		//------------------------------------------------------------------------------------------------
					//	ここから書き込むバックバッファのインデックスを取得
		UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

		//	TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrier{};
		//	今回のバリアはTransition
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//	Noneにしておく
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//	バリアを張る対象のリソース。現在のバックバッファに対して行う
		barrier.Transition.pResource = swapChainResources[backBufferIndex];
		//	遷移前（現在）のResourceState
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//	遷移後のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//	TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

		//	描画先のRTVとDSVを設定する
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle[backBufferIndex], false, &dsvHandle);
		//	指定した深度で画面全体をクリアする
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//	指定した色で画面全体をクリアする
		float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	//	青っぽい色、RGBA
		commandList->ClearRenderTargetView(rtvHandle[backBufferIndex], clearColor, 0, nullptr);

		//	描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
		commandList->SetDescriptorHeaps(1, descriptorHeaps);

		//	Viewportを設定
		commandList->RSSetViewports(1, &viewport);
		//	Scirssorを設定
		commandList->RSSetScissorRects(1, &scissorRect);
		//	RootSignatureを設定。PSOに設定してるけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature);
		//	PSOを設定
		commandList->SetPipelineState(graphicsPipelineState);
		//	VBVを設定
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		//	形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//	マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		//	wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		//	SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
		//	描画。（DrawCall/ドローコール）。3頂点で1つのインスタンス。インスタンスについては今後
		commandList->DrawInstanced(6, 1, 0, 0);
		//	Spriteの描画。変更が必要なものだけ変更する
		commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);	//	VBVを設定
		//	TransformationMatrixCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
		//	描画(DrawCall/ドローコール)
		commandList->DrawInstanced(6, 1, 0, 0);

		//	実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);


		//	画面に描く処理はすべて終わり、画面に映すので、状態を遷移
		//	今回はRenderTargetからPresentにする
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		//	TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

		//	コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseする
		hr = commandList->Close();
		assert(SUCCEEDED(hr));

		//	GPUにコマンドリストの実行を行わせる
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);
		//	GPUとOSに画面の交換を行うよう通知する
		swapChain->Present(1, 0);

		//	Fenceの値を更新
		fenceValue++;
		//	GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue->Signal(fence, fenceValue);
		//	Fenceの値が指定したSignal値にたどり着いているか確認する
		//	GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceValue)
		{
			//	FenceのSignalを持つためのイベントを作成する
			HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(fenceEvent != nullptr);
			//	指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			//	イベント待つ
			WaitForSingleObject(fenceEvent, INFINITE);
			//	解放
			CloseHandle(fenceEvent);
		}

		//	次のフレーム用のコマンドリストを用意
		hr = commandAllocator->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList->Reset(commandAllocator, nullptr);
		assert(SUCCEEDED(hr));
	}

	//
	depthStencilResource->Release();
	dsvDescriptorHeap->Release();
	transformationMatrixResourceSprite->Release();
	vertexResourceSprite->Release();


	//	Textureの解放
	intermediateResource->Release();
	textureResource->Release();

	//	ImGuiの解放
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//	解放処理
	
	fence->Release();
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif
	CloseWindow(hwnd);

	wvpResource->Release();
	vertexResource->Release();
	materialResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();


	//	リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
	
	Engine::Finalize();

	return 0;
}

*/

#include "Texture2D.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("Engine", windowWidth, windowHeight);

	Texture2D texture;
	texture.LoadTexture("./Resources/uvChecker.png");
	texture.CreateDescriptor();
	texture.CreateShader("Texture2D.VS.hlsl", "Texture2D.PS.hlsl");
	texture.CreateGraphicsPipeline();

	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();

		texture.Draw();

		//	フレームの終了
		Engine::EndFrame();
	}

	texture.Finalize();
	Engine::Finalize();

	return 0;
}