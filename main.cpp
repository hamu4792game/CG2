#include <Windows.h>
#include "Engine/Engine.h"
#include "Engine/Texture/Texture2D.h"
#include "externals/imgui/imgui.h"
#include "math/Matrix4x4.h"
#include "Engine/Camera/Camera.h"

#include "Engine/Texture/Model.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include "Engine/Input/AudioInput/AudioInput.h"

#include "Game/GameScene/GameScene.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	static D3DResourceLeakChecker leak;
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("taimenn", windowWidth, windowHeight);
	
	GameScene::GetInstance()->Initialize();

	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();

		//	ImGui のフレームに一つ目の ImGui のウィンドウを描く
		ImGui::Begin("Control panel");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		ImGui::End();


		GameScene::GetInstance()->Update();

		GameScene::GetInstance()->Draw();

		

		//	フレームの終了
		Engine::EndFrame();
		if (KeyInput::PushKey(DIK_ESCAPE)) {
			return 0;
		}
	}

	Engine::Finalize();

	return 0;
}