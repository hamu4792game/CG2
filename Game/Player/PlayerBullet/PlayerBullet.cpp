#include "Game/Player/PlayerBullet/PlayerBullet.h"


void PlayerBullet::Initialize(const Vector3& vector, const WorldTransform& player)
{
	//transform.parent_ = &player;
	transform.translation_ = player.translation_;
	transform.translation_.y += 5.0f;
	models_ = std::make_unique<Model>();

	//	移動量の正規化 * speed
	move = vector * 0.5f;
	ModelLoad();
}

void PlayerBullet::ModelLoad()
{
	models_->Texture("Resources/sphere.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void PlayerBullet::Update()
{
	//	移動量の追加
	transform.translation_ += move;

	//	更新
	transform.UpdateMatrix();
}

void PlayerBullet::Draw(const Matrix4x4& viewProjection)
{
	models_->Draw(transform, viewProjection, 0xffffffff);
}
