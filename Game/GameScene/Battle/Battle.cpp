#include "Game/GameScene/GameScene.h"
#include "Battle.h"
#include "Game/GameScene/GameScene.h"
#include "Engine/Input/KeyInput/KeyInput.h"


Battle::Battle(std::shared_ptr<Camera> camera)
{
	camera_ = camera;
	player = std::make_unique<Player>();
	enemy = std::make_unique<Enemy>();
	hud = std::make_unique<Hud>();
}

void Battle::Initialize()
{

	//	playerのset
	
	player->SetViewProjection(camera_.get());
	player->Initialize();
	
	enemy->Initialize();

	//	playerの角度セット
	player->SetEnemy_ptr(enemy.get());
	enemy->SetPlayer_ptr(player.get());

	hud->Initialize();
	hud->SetPlayer_ptr(player.get());
	hud->SetEnemy_ptr(enemy.get());

}

void Battle::ModelLoad()
{
	player->ModelLoad();
	enemy->ModelLoad();
	hud->TextureLoad();
}

void Battle::Update()
{
	enemy->Update();
	player->Update();
	hud->Update();
	player->CameraMove();
	if (enemy->clearFlag) {
		GameScene::GetInstance()->sceneChangeFlag = true;
	}
}

void Battle::Draw(const Matrix4x4& viewProjection)
{
	player->Draw(viewProjection);
	enemy->Draw(viewProjection);	
}

void Battle::Draw2d(const Matrix4x4& viewProjection2d)
{
	hud->Draw(viewProjection2d);
}