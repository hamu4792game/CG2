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

	//	カメラの生成
	Camera camera(1000.0f, true);
	Camera spriteCamera(1000.0f, false);
	//	カメラ座標
	Vector3 cameraTranslate = { 0.0f,0.0f,-10.0f };
	Vector3 cameraRotate = { 0.0f,0.0f,0.0f };
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};


	Vector3 worldTranslate = { 0.0f,0.0f,0.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	
	Vector2 spriteScale = { 1.1f,0.7f };
	float spriteRotate = { 0.0f };
	Vector2 spriteWorldTranslate = { -360.0f,180.0f };
	bool spriteFlag = false;
	uint32_t spriteColor = 0xffffffff;

	Vector3 multiMaterialScale = { 1.0f,1.0f,1.0f };
	Vector3 multiMaterialRotate = { 0.0f };
	Vector3 multiMaterialWorldTranslate = { 0.0f };
	bool multiMaterialFlag = false;
	uint32_t multiMaterialColor = 0xffffffff;

	Vector3 teapotScale = { 1.0f,1.0f };
	Vector3 teapotRotate = { 0.0f };
	Vector3 teapotWorldTranslate = { 0.0f };
	bool teapotFlag = false;
	uint32_t teapotColor = 0xffffffff;
	
	Vector3 bunnyScale = { 1.0f,1.0f };
	Vector3 bunnyRotate = { 0.0f };
	Vector3 bunnyWorldTranslate = { 0.0f };
	bool bunnyFlag = false;
	uint32_t bunnyColor = 0xffffffff;
	

	//	モデル読み込み
	auto multiMaterial = std::make_unique<Model>();
	multiMaterial->Texture("Resources/multiMaterial.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	auto teapot = std::make_unique<Model>();
	teapot->Texture("Resources/teapot.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	auto bunny = std::make_unique<Model>();
	bunny->Texture("Resources/bunny.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	auto sphere = std::make_unique<Model>();
	bunny->Sphere("Resources/uvChecker.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

	//	テクスチャ
	auto texture = std::make_unique<Texture2D>();
	texture->Texture("Resources/uvChecker.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	
	//	ウィンドウの×ボタンが押されるまでループ
	while (!WinApp::ProcessMessage()) {
		//	フレームの開始
		Engine::BeginFrame();

		// ImGui のフレームに一つ目の ImGui のウィンドウを描く
		ImGui::Begin("Control panel");
		ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		ImGui::DragFloat3("worldMatrix", &worldTranslate.x, 1.0f);
		ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);

		ImGui::Checkbox("Sptite", &spriteFlag);
		if (spriteFlag)	{
			if (ImGui::TreeNode("Sprite")) {

				ImGui::DragFloat2("SpriteTranslate", &spriteWorldTranslate.x, 1.0f);
				ImGui::DragFloat2("SpriteScale", &spriteScale.x, 0.1f, 0.0f);
				ImGui::DragFloat("SpriteRotate", &spriteRotate, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("MultiMaterial", &multiMaterialFlag);
		if (multiMaterialFlag) {
			if (ImGui::TreeNode("MultiMaterial")) {

				ImGui::DragFloat3("MultiMaterialTranslate", &multiMaterialWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("MultiMaterialScale", &multiMaterialScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("MultiMaterialRotate", &multiMaterialRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("Teapot", &teapotFlag);
		if (teapotFlag)	{
			if (ImGui::TreeNode("Teapot")) {
				ImGui::DragFloat3("TeapotTranslate", &teapotWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("TeapotScale", &teapotScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("TeapotRotate", &teapotRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("Bunny", &bunnyFlag);
		if (bunnyFlag)	{
			if (ImGui::TreeNode("Bunny")) {
				ImGui::DragFloat3("BunnyTranslate", &bunnyWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("BunnyScale", &bunnyScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("BunnyRotate", &bunnyRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::TreePop();
			}
		}

		ImGui::End();


		//	行列の計算 レンダリングパイプライン
		viewProjectionMatrix = spriteCamera.GetViewProMat(cameraTranslate);
		//	スプライトの描画
		if (spriteFlag)	{
			texture->Draw(spriteWorldTranslate, spriteScale, spriteRotate, viewProjectionMatrix, spriteColor);
		}
		
		//	モデルの切り替え
		viewProjectionMatrix = camera.GetViewProMat(cameraTranslate);
		//	モデルの描画
		if (multiMaterialFlag) {
			multiMaterial->Draw(multiMaterialWorldTranslate, multiMaterialScale, multiMaterialRotate, viewProjectionMatrix, multiMaterialColor);
		}
		if (teapotFlag)	{
			teapot->Draw(teapotWorldTranslate, teapotScale, teapotRotate, viewProjectionMatrix, teapotColor);
		}
		if (bunnyFlag)	{
			bunny->Draw(bunnyWorldTranslate, bunnyScale, bunnyRotate, viewProjectionMatrix, bunnyColor);
		}

		sphere->Draw(bunnyWorldTranslate, bunnyScale, bunnyRotate, viewProjectionMatrix, bunnyColor);

		//	フレームの終了
		Engine::EndFrame();
	}

	//texture->Finalize();
	//texture.reset();
	Engine::Finalize();

	return 0;
}