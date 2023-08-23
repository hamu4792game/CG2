#include "Player.h"
#include "externals/imgui/imgui.h"
#include "math/Vector3.h"
#include "math/Matrix4x4.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include <cmath>
#include <algorithm>

void Player::Initialize()
{
	transform.translation_ = Vector3(0.0f, 0.0f, -40.0f);

	for (uint16_t i = 0; i < num; i++)
	{
		models_.push_back(std::make_unique<Model>());
	}

	parts_.resize(models_.size());
	for (auto& i : parts_) {
		i.parent_ = &parts_[0];
	}
	parts_[0].parent_ = &transform;

	parts_[1].translation_ = { 0.0f,1.5f,0.0f };

	color = 0xffffffff;

	ModelLoad();

}

void Player::Update()
{
	//	弾更新
	for (auto i = bullets_.begin(); i != bullets_.end(); i++) {
		(*i)->Update();
	}

	transform.UpdateMatrix();
	for (auto& i : parts_) {
		i.UpdateMatrix();
	}
}

void Player::ModelLoad()
{
	
	models_[0]->Texture("Resources/float_Body.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[1]->Texture("Resources/float_Head.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[2]->Texture("Resources/float_L_arm.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[3]->Texture("Resources/float_R_arm.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

}

void Player::Move()
{
	ImGui::DragFloat3("PlayerTranslate", &transform.translation_.x, 0.1f);
	Vector3 move = { 0.0f,0.0f,0.0f };
	const float speed = 0.2f;

	if (KeyInput::GetKey(DIK_W))
	{
		move.z += speed;
	}
	if (KeyInput::GetKey(DIK_S))
	{
		move.z -= speed;
	}
	if (KeyInput::GetKey(DIK_D))
	{
		move.x += speed;
	}
	if (KeyInput::GetKey(DIK_A))
	{
		move.x -= speed;
	}

	//	移動があれば更新
	if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f)
	{
		//	移動量の正規化 * speed
		move = Normalize(move) * speed;
		//	移動ベクトルをカメラの角度だけ回転させる
		move = TransformNormal(move, MakeRotateMatrix(camera->transform.rotation_));
			
		//	移動方向に見た目を合わせる
		transform.rotation_.y = std::atan2f(move.x, move.z);

		//	敵の方向を見続ける
		//transform.rotation_.y = camera->degree;
	}
	//	座標移動（ベクトルの加算）
	if (fabs(transform.translation_.x + move.x) <= 1.0f && fabs(transform.translation_.z + move.z) <= 1.0f) {
		transform.translation_ += oldMove;
	}
	else {
		transform.translation_ += move;
	}
	oldMove = move;

	//	移動制限
	MoveLimit();

	カメラのパラメーターをプレイヤーで操作してカメラの更新はCamera.Updateにやらせるように変更する

}

void Player::Draw(const Matrix4x4& viewProjection)
{
	for (uint16_t i = 0u; i < parts_.size(); i++)
	{
		models_[i]->Draw(parts_[i], viewProjection, color);
	}
	for (auto i = bullets_.begin(); i != bullets_.end(); i++) {
		(*i)->Draw(viewProjection);
	}
}

void Player::MoveLimit()
{
	transform.translation_.x = std::clamp(transform.translation_.x, -50.0f, 50.0f);
	transform.translation_.z = std::clamp(transform.translation_.z, -50.0f, 50.0f);
	//transform.translation_.y = 0.0f;
}

void Player::Attack(const Vector3& distance)
{
	if (KeyInput::PushKey(DIK_SPACE)) {
		//if (OuterProduct(distance) >= 10.0f) {
			bullets_.push_back(std::make_unique<PlayerBullet>());
			(*bullets_.rbegin())->Initialize(distance, transform);
		//}
	}
}
