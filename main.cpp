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
	
	Vector2 spriteScale = { 1.1f,0.7f };
	float spriteRotate = { 0.0f };
	Vector2 spriteWorldTranslate = { -360.0f,180.0f };
	bool spriteFlag = false;
	Vector4 spriteColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t spriteColor = 0xffffffff;

	Vector3 multiMeshScale = { 1.0f,1.0f,1.0f };
	Vector3 multiMeshRotate = { 0.0f };
	Vector3 multiMeshWorldTranslate = { 0.0f };
	bool multiMeshFlag = false;
	Vector4 multiMeshColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t multiMeshColor = 0xffffffff;

	Vector3 teapotScale = { 1.0f,1.0f,1.0f };
	Vector3 teapotRotate = { 0.0f };
	Vector3 teapotWorldTranslate = { 0.0f };
	bool teapotFlag = false;
	Vector4 teapotColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t teapotColor = 0xffffffff;
	
	Vector3 bunnyScale = { 1.0f,1.0f,1.0f };
	Vector3 bunnyRotate = { 0.0f };
	Vector3 bunnyWorldTranslate = { 0.0f };
	bool bunnyFlag = false;
	Vector4 bunnyColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t bunnyColor = 0xffffffff;
	
	Vector3 sphereScale = { 1.0f,1.0f,1.0f };
	Vector3 sphereRotate = { 0.0f };
	Vector3 sphereWorldTranslate = { 0.0f };
	bool sphereFlag = false;
	Vector4 sphereColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t sphereColor = 0xfffffffff;
	
	Vector3 triangleScale = { 1.0f,1.0f,1.0f };
	Vector3 triangleRotate = { 0.0f };
	Vector3 triangleWorldTranslate = { 0.0f };
	bool triangleFlag = false;
	Vector4 triangleColor = { 255.0f,255.0f,255.0f,255.0f };
	//uint32_t triangleColor = 0xfffffffff;
	

	//	モデル読み込み
	auto multiMesh = std::make_unique<Model>();
	multiMesh->Texture("Resources/multiMesh.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	auto teapot = std::make_unique<Model>();
	teapot->Texture("Resources/teapot.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	auto bunny = std::make_unique<Model>();
	bunny->Texture("Resources/bunny.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	
	auto sphere = std::make_unique<Model>();
	sphere->Sphere("Resources/uvChecker.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

	auto triangle = std::make_unique<Model>();
	triangle->Triangle("Resources/uvChecker.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

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
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.1f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::Checkbox("Sptite", &spriteFlag);
		if (spriteFlag)	{
			if (ImGui::TreeNode("Sprite")) {

				ImGui::DragFloat2("SpriteTranslate", &spriteWorldTranslate.x, 1.0f);
				ImGui::DragFloat2("SpriteScale", &spriteScale.x, 0.1f, 0.0f);
				ImGui::DragFloat("SpriteRotate", &spriteRotate, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::DragFloat4("SpriteColor", &spriteColor.x, 1.0f, 0.0f, 255.0f);
				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("MultiMesh", &multiMeshFlag);
		if (multiMeshFlag) {
			if (ImGui::TreeNode("MultiMesh")) {

				ImGui::DragFloat3("MultiMeshTranslate", &multiMeshWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("MultiMeshScale", &multiMeshScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("MultiMeshRotate", &multiMeshRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::DragFloat4("MultiMeshColor", &multiMeshColor.x, 1.0f, 0.0f, 255.0f);
				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("Teapot", &teapotFlag);
		if (teapotFlag)	{
			if (ImGui::TreeNode("Teapot")) {
				ImGui::DragFloat3("TeapotTranslate", &teapotWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("TeapotScale", &teapotScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("TeapotRotate", &teapotRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::DragFloat4("TeapotColor", &teapotColor.x, 1.0f, 0.0f, 255.0f);
				ImGui::TreePop();
			}
		}
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
		ImGui::Checkbox("Sphere", &sphereFlag);
		if (sphereFlag)	{
			if (ImGui::TreeNode("Sphere")) {
				ImGui::DragFloat3("SphereTranslate", &sphereWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("SphereScale", &sphereScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("SphereRotate", &sphereRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::DragFloat4("SphereColor", &sphereColor.x, 1.0f, 0.0f, 255.0f);
				//ImGui::ColorEdit4("SphereColor", &sphereColor.x);
				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("Triangle", &triangleFlag);
		if (triangleFlag)	{
			if (ImGui::TreeNode("Triangle")) {
				ImGui::DragFloat3("TriangleTranslate", &triangleWorldTranslate.x, 0.1f);
				ImGui::DragFloat3("TriangleScale", &triangleScale.x, 0.1f, 0.0f);
				ImGui::DragFloat3("TriangleRotate", &triangleRotate.x, 0.1f, -3.14f * 2.0f, 3.14f * 2.0f);
				ImGui::DragFloat4("TriangleColor", &triangleColor.x, 1.0f, 0.0f, 255.0f);
				ImGui::TreePop();
			}
		}

		ImGui::End();


		//	行列の計算 レンダリングパイプライン
		viewProjectionMatrix = spriteCamera.GetViewProMat();
		//	スプライトの描画
		if (spriteFlag)	{
			texture->Draw(spriteWorldTranslate, spriteScale, spriteRotate, viewProjectionMatrix, TextureManager::ChangeColor(spriteColor));
		}
		
		//	モデルの切り替え
		viewProjectionMatrix = camera.GetViewProMat(cameraTranslate, cameraRotate);
		//	モデルの描画
		if (multiMeshFlag) {
			multiMesh->Draw(multiMeshWorldTranslate, multiMeshScale, multiMeshRotate, viewProjectionMatrix, TextureManager::ChangeColor(multiMeshColor));
		}
		if (teapotFlag)	{
			teapot->Draw(teapotWorldTranslate, teapotScale, teapotRotate, viewProjectionMatrix, TextureManager::ChangeColor(teapotColor));
		}
		if (bunnyFlag)	{
			bunny->Draw(bunnyWorldTranslate, bunnyScale, bunnyRotate, viewProjectionMatrix, TextureManager::ChangeColor(bunnyColor));
		}
		if (sphereFlag)	{
			sphere->Draw(sphereWorldTranslate, sphereScale, sphereRotate, viewProjectionMatrix, TextureManager::ChangeColor(sphereColor));
		}
		if (triangleFlag)	{
			triangle->Draw(triangleWorldTranslate, triangleScale, triangleRotate, viewProjectionMatrix, TextureManager::ChangeColor(triangleColor));
		}

		//	フレームの終了
		Engine::EndFrame();
	}

	Engine::Finalize();

	return 0;
}