#include "GameScene.h"
#include "Battle/Battle.h"
#include <externals/imgui/imgui.h>

GameScene* GameScene::GetInstance()
{
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize()
{
	//	モデルの読み込みと生成
	skydome = std::make_unique<Skydome>("Resources/box.obj");
	ground = std::make_unique<Ground>("Resources/ground.obj");
	camera = std::make_shared<Camera>(2000.0f, true);

	//	初期化
	battle.Initialize(camera);
}

void GameScene::Update()
{

	ImGui::DragFloat3("CameraTranslate", &camera->transform.translation_.x, 0.1f);
	ImGui::DragFloat3("CameraRotate", &camera->transform.rotation_.x, 0.01f);

	switch (scene)
	{
	case GameScene::Scene::TITLE:
		break;
	case GameScene::Scene::BATTLE:
		battle.Update();

		break;
	case GameScene::Scene::RESULT:
		break;
	}

	//	カメラ行列の更新
	viewProjectionMatrix = camera->GetViewProMat();
}

void GameScene::Draw()
{
	skydome->Draw(viewProjectionMatrix);
	ground->Draw(viewProjectionMatrix);
	
	switch (scene)
	{
	case GameScene::Scene::TITLE:
		break;
	case GameScene::Scene::BATTLE:
		battle.Draw(viewProjectionMatrix);
		break;
	case GameScene::Scene::RESULT:
		break;
	default:
		break;
	}
}
