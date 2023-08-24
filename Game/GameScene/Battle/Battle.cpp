#include "Game/GameScene/GameScene.h"
#include "Battle.h"
#include "Game/GameScene/GameScene.h"
#include "Engine/Input/KeyInput/KeyInput.h"


void Battle::Initialize(std::shared_ptr<Camera> camera)
{
	camera_ = camera;

	//	playerのset
	player = std::make_unique<Player>();
	player->Initialize();
	enemy = std::make_unique<Enemy>();
	enemy->Initialize();

	//	playerの角度セット
	player->SetViewProjection(camera_.get());
	player->SetEnemy_ptr(enemy.get());
	camera_->SetTarget(&player->GetWorldTransform());
	camera_->SetLockon(&enemy->GetWorldTransform());
}

void Battle::Update()
{
	enemy->Update();
	player->Update();
	if (KeyInput::PushKey(DIK_L))
	{
		/*GameScene::GetInstance()->scene = GameScene::Scene::RESULT;*/
	}
	player->CameraMove();
}

void Battle::Draw(const Matrix4x4& viewProjection)
{
	player->Draw(viewProjection);
	enemy->Draw(viewProjection);
}
