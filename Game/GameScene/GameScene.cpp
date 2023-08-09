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
	player->Initialize();
	skydome = std::make_unique<Skydome>("Resources/boxSky.obj");
	camera = std::make_unique<Camera>(100.0f, true);

	//	playerの角度セット
	player->SetViewProjection(camera.get());
	camera->SetTarget(&player->GetWorldTransform());
}

void GameScene::Update()
{

	ImGui::DragFloat3("CameraTranslate", &camera->translate.x, 0.1f);
	ImGui::DragFloat3("CameraRotate", &camera->rotate.x, 0.01f);

	switch (scene)
	{
	case GameScene::Scene::TITLE:
		break;
	case GameScene::Scene::BATTLE:
		battle.Update();
		player->Update();

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
	player->Draw(viewProjectionMatrix);
	
}
