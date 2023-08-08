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
	player = std::make_unique<Player>();
	player->ModelLoad();
	skydome = std::make_unique<Skydome>("Resources/boxSky.obj");
	camera = std::make_unique<Camera>(100.0f, true);

}

void GameScene::Update()
{

	ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.1f);
	ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

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
	viewProjectionMatrix = camera->GetViewProMat(cameraTranslate, cameraRotate);

}

void GameScene::Draw()
{
	

	skydome->Draw(viewProjectionMatrix);
}
