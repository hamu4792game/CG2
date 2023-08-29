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

	
	hud->Initialize();

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
	player->CameraMove();
	if (KeyInput::PushKey(DIK_L))
	{
		/*GameScene::GetInstance()->scene = GameScene::Scene::RESULT;*/
		Initialize();
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