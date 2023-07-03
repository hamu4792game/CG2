#include <Windows.h>
#include "Engine.h"
#include "Log.h"
#include "Texture2D.h"
#include "externals/imgui/imgui.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("Engine", windowWidth, windowHeight);

	auto texture = std::make_unique<Texture2D>();
	texture->Texture("./Resources/textureA.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2DAbsolute.PS.hlsl");
	//auto texture1 = std::make_unique<Texture2D>();
	//texture->Texture("./Resources/zeno.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

	uint32_t color = 0xffffffff;

	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();
		
		//ImGui::DragInt("%d", &color);
		//texture1->SetBlend(BlendMode::Normal);
		//texture1->Draw(color);
		//texture->SetBlend(BlendMode::Normal);
		texture->Draw(color);

		// ImGui のフレームに一つ目の ImGui のウィンドウを描く
		ImGui::Begin("Control panel");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		ImGui::End();

		//	フレームの終了
		Engine::EndFrame();
	}

	//texture1.reset();
	texture.reset();
	Engine::Finalize();

	return 0;
}