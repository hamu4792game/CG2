#include <Windows.h>
#include "Engine.h"
#include "Log.h"
#include "Texture2D.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("Engine", windowWidth, windowHeight);

	auto texture = std::make_unique<Texture2D>();
	texture->Texture("./Resources/uvChecker.png", "Texture2D.VS.hlsl", "Texture2D.PS.hlsl");
	//auto texture1 = std::make_unique<Texture2D>();
	//texture1->Texture("./Resources/517-a.png", "Texture2D.VS.hlsl", "Texture2D.PS.hlsl");

	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();
		
		texture->Draw();
		//texture1->Draw();

		//	フレームの終了
		Engine::EndFrame();
	}

	texture.reset();
	//texture1.reset();
	Engine::Finalize();

	return 0;
}