#include <Windows.h>
#include "Engine/Engine.h"
#include "Engine/Texture/Texture2D.h"
#include "externals/imgui/imgui.h"
#include "math/Matrix4x4.h"
#include "Engine/Camera/Camera.h"

#include "Engine/Texture/Model.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	D3DResourceLeakChecker leak;
	//OutputDebugStringA("Hello,DirectX!\n");
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Engine::Initialize("Engine", windowWidth, windowHeight);

	//auto texture = std::make_unique<Texture2D>();
	//texture->Texture("./Resources/textureA.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	
	Vector3 worldTranslate = { 0.0f,0.0f,0.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	//	カメラの生成
	Camera camera(1000.0f, true);
	//	カメラ座標
	Vector3 cameraTranslate = { 0.0f,0.0f,-10.0f };
	Vector3 cameraRotate = { 0.0f,0.0f,0.0f };
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};

	uint32_t color = 0xffffffff;

	//	モデル読み込み
	auto model = std::make_unique<Model>();
	model->Texture("Resources/Box.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	
	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();

		// ImGui のフレームに一つ目の ImGui のウィンドウを描く
		ImGui::Begin("Control panel");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		ImGui::DragFloat3("worldMatrix", &worldTranslate.x, 1.0f);
		ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
		ImGui::End();

		//	行列の計算 レンダリングパイプライン
		viewProjectionMatrix = camera.GetViewProMat(cameraTranslate);

		//texture->Draw(worldMatrix, viewProjectionMatrix, color);
		//texture->Draw(worldTranslate, { 1.0f,1.0f }, rotate, viewProjectionMatrix, 0xffffffff);

		model->Draw(worldTranslate, { 1.0f,1.0f,1.0f }, rotate, viewProjectionMatrix, 0xffffffff);

		//	フレームの終了
		Engine::EndFrame();
	}

	//texture->Finalize();
	//texture.reset();
	Engine::Finalize();

	return 0;
}