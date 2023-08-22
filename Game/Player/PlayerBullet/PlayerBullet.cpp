#include "Game/Player/PlayerBullet/PlayerBullet.h"
#include "Game/Player/Player.h"


void PlayerBullet::Initialize(const Vector3& camerarotate)
{
	transform.parent_ = &player->GetWorldTransform();
	transform.translation_.y = 10.0f;
	models_ = std::make_unique<Model>();

	//	移動量の正規化 * speed
	move = { 0.0f,0.0f,0.2f };
	move = Normalize(move) * 0.2f;
	//	移動ベクトルをカメラの角度だけ回転させる
	move = TransformNormal(move, MakeRotateMatrix(camerarotate));

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
