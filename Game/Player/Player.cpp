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
		i.parent_ = &parts_[Body];
	}
	parts_[Body].parent_ = &transform;

	parts_[Head].translation_ = { 0.0f,1.5f,0.0f };

	color = 0xffffffff;

	ModelLoad();

}

void Player::Update()
{
	//	弾更新
	for (auto i = bullets_.begin(); i != bullets_.end(); i++) {
		(*i)->Update(enemy_->transform.translation_);
	}

	transform.UpdateMatrix();
	for (auto& i : parts_) {
		i.UpdateMatrix();
	}
}

void Player::ModelLoad()
{
	
	models_[Body]->Texture("Resources/float_Body.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[Head]->Texture("Resources/float_Head.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[L_arm]->Texture("Resources/float_L_arm.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[R_arm]->Texture("Resources/float_R_arm.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

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
		oldMove = move;
	}
	

	//	移動制限
	MoveLimit();

	//カメラのパラメーターをプレイヤーで操作してカメラの更新はCamera.Updateにやらせるように変更する

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
	transform.translation_.y = 0.0f;
}

void Player::Attack(const Vector3& distance)
{
	//	生存フラグが折れたら要素を取り除く
	std::erase_if(bullets_, [](std::unique_ptr<PlayerBullet>& bullet) {return bullet->isAlive == false; });
	ImGui::Text("%f", OuterProduct(distance));
	if (KeyInput::PushKey(DIK_SPACE)) {
		if (OuterProduct(distance) >= 15.0f) {
		//	弾を追加する
			bullets_.push_back(std::make_unique<PlayerBullet>());
			(*bullets_.rbegin())->Initialize(distance, transform);
		}
	}
}

void Player::CameraMove()
{
	if (true)
	{
		//エネミーからプレイヤーに伸びるベクトルを求めます。
		Vector3 pos = transform.translation_ - enemy_->transform.translation_;
		//カメラの高さは一定にしたいので、y成分を0にします。
		pos.y = 0.0f;
		//ベクトルを正規化します。
		pos = Normalize(pos);
		//スカラーを掛けます
		pos *= 40.0f;
		//カメラがどれだけプレイヤーの座標より高いかを設定します。
		pos.y = 10.0f;
		//プレイヤーの座標に求めたベクトルを足して、カメラの座標とします。
		camera->position = transform.translation_ + pos;
		//	カメラを回転してあげる 逆ベクトルなので-
		camera->transform.rotation_.y = atan2f(-camera->position.x, -camera->position.z);
		//transform.rotation_.x = atan2f(-position.y, -position.z);

		//	座標の反映
		camera->transform.translation_ = camera->position;
	}
}
