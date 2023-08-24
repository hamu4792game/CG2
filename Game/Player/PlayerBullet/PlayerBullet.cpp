#include "Game/Player/PlayerBullet/PlayerBullet.h"


void PlayerBullet::Initialize(const Vector3& vector, const WorldTransform& player)
{
	//transform.parent_ = &player;
	transform.translation_ = player.translation_;
	transform.translation_.y += 0.0f;
	models_ = std::make_unique<Model>();

	//	移動量の正規化 * speed
	velocity = Normalize(vector) * 0.5f;
	isAlive = true;
	ModelLoad();
}

void PlayerBullet::ModelLoad()
{
	models_->Texture("Resources/sphere.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void PlayerBullet::Update(const Vector3& vector)
{
	//	対象(enemy)までの距離を取得し、フラグを立てる
	if (OuterProduct(transform.translation_ - vector) <= 1.0f) {
		isAlive = false;
	}
	if (isAlive) {
		//	移動量の追加
		transform.translation_ += velocity;
	}
	else {
		transform.translation_ = Vector3(0.0f, -50.0f, 0.0f);
	}
	
	//	更新
	transform.UpdateMatrix();
}

void PlayerBullet::Draw(const Matrix4x4& viewProjection)
{
	if (isAlive) {
		models_->Draw(transform, viewProjection, 0xffffffff);
	}
}
