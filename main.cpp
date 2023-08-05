#include <Windows.h>
#include "Engine/Engine.h"
#include "Engine/Texture/Texture2D.h"
#include "externals/imgui/imgui.h"
#include "math/Matrix4x4.h"
#include "Engine/Camera/Camera.h"

#include "Engine/Texture/Model.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include "Engine/Input/AudioInput/AudioInput.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	D3DResourceLeakChecker leak;
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("Engine", windowWidth, windowHeight);

	//	カメラの生成
	Camera camera(1000.0f, true);
	Camera spriteCamera(1000.0f, false);
	//	カメラ座標
	Vector3 cameraTranslate = { 0.0f,0.0f,-10.0f };
	Vector3 cameraRotate = { 0.0f,0.0f,0.0f };
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};
	
	Vector3 bunnyScale = { 1.0f,1.0f,1.0f };
	Vector3 bunnyRotate = { 0.0f };
	Vector3 bunnyWorldTranslate = { 0.0f };
	bool bunnyFlag = false;
	Vector4 bunnyColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t bunnyColor = 0xffffffff;
	
	//	モデル読み込み
	
	auto bunny = std::make_unique<Model>();
	bunny->Texture("Resources/plane.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

	auto oto = std::make_unique<AudioInput>();
	oto->SoundLoadWave("./Resources/sound/Alarm01.wav");

	AudioManager::GetInstance()->Initialize();
	
	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();

		// ImGui のフレームに一つ目の ImGui のウィンドウを描く
		ImGui::Begin("Control panel");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.1f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::Checkbox("Bunny", &bunnyFlag);
		if (bunnyFlag)	{
			if (ImGui::TreeNode("Bunny")) {
				ImGui::DragFloat3("BunnyTranslate", &bunnyWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("BunnyScale", &bunnyScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("BunnyRotate", &bunnyRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::DragFloat4("BunnyColor", &bunnyColor.x, 1.0f, 0.0f, 255.0f);
				ImGui::TreePop();
			}
		}
		ImGui::End();


		if (KeyInput::PushKey(DIK_SPACE))
		{
			oto->SoundPlayWave();
		}if (KeyInput::PushKey(DIK_D))
		{
			oto->ReStart();
		}
		if (KeyInput::PushKey(DIK_A))
		{
			//AudioInput::SoundPlayWave(soundData2);
			oto->Pose();
		}

		//	行列の計算 レンダリングパイプライン
		viewProjectionMatrix = spriteCamera.GetViewProMat();
		//	スプライトの描画
		

		//	モデルの切り替え
		viewProjectionMatrix = camera.GetViewProMat(cameraTranslate, cameraRotate);
		if (bunnyFlag)	{
			bunny->Draw(bunnyWorldTranslate, bunnyScale, bunnyRotate, viewProjectionMatrix, TextureManager::ChangeColor(bunnyColor));
		}

		//	フレームの終了
		Engine::EndFrame();
		if (KeyInput::PushKey(DIK_ESCAPE)) {
			return 0;
		}
	}

	Engine::Finalize();

	return 0;
}