#include "GameScene.h"
#include "Battle/Battle.h"
#include <externals/imgui/imgui.h>
#include "Engine/Input/KeyInput/KeyInput.h"

GameScene* GameScene::GetInstance()
{
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize()
{
	//	モデルの読み込みと生成
	skydome = std::make_unique<Skydome>();
	skydome->ModelLoad();
	ground = std::make_unique<Ground>("Resources/ground/ground.obj");
	camera = std::make_shared<Camera>(2000.0f, true);
	camera2d = std::make_unique<Camera>();
	//	カメラ行列の更新
	viewProjectionMatrix = camera->GetViewProMat();
	viewProjectionMatrix2d = camera2d->GetViewProMat();

	battleBGM.SoundLoadWave("./Resources/sound/w006.wav");
	battleBGM.SoundPlayWave(true);
	battleBGM.SetVolume(0.3f);

	//	初期化
	battle = std::make_unique<Battle>(camera);
	battle->Initialize();
	battle->ModelLoad();
}

void GameScene::Update()
{

	ImGui::DragFloat3("CameraTranslate", &camera->transform.translation_.x, 0.1f);
	ImGui::DragFloat3("CameraRotate", &camera->transform.rotation_.x, 0.01f);

	

	switch (scene)
	{
	case GameScene::Scene::TITLE:
		scene = Scene::BATTLE;
		break;
	case GameScene::Scene::BATTLE:
		battle->Update();
		
		break;
	case GameScene::Scene::RESULT:
		break;
	}

	//	カメラ行列の更新
	viewProjectionMatrix = camera->GetViewProMat();
	viewProjectionMatrix2d = camera2d->GetViewProMat();
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
		battle->Draw(viewProjectionMatrix);
		battle->Draw2d(viewProjectionMatrix2d);
		break;
	case GameScene::Scene::RESULT:
		break;
	default:
		break;
	}
}
