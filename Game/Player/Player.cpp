#include "Player.h"
#include "externals/imgui/imgui.h"
#include "math/Vector3.h"
#include "math/Matrix4x4.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include <cmath>
#include <algorithm>
#include <numbers>

void Player::Initialize()
{
	transform.translation_ = Vector3(0.0f, 0.0f, -40.0f);

	for (uint16_t i = 0; i < PARTS::Num; i++)
	{
		models_.push_back(std::make_unique<Model>());
	}

	parts_.resize(models_.size());
	for (auto& i : parts_) {
		i.parent_ = &parts_[Body];
	}
	parts_[Body].parent_ = &transform;

	parts_[Body].translation_ = { 0.0f,2.0f,0.0f };

	parts_[Head].translation_ = { 0.0f,2.1f,0.0f };
	parts_[L_arm].translation_ = { 1.3f,1.5f,0.0f };
	parts_[R_arm].translation_ = { -1.3f,1.5f,0.0f };
	parts_[L_leg].translation_ = { 0.5f,0.0f,0.0f };
	parts_[R_leg].translation_ = { -0.5f,0.0f,0.0f };

	color = 0xffffffff;
	distance = 0.0f;


	ModelLoad();

}

void Player::Update()
{
	//	
	distance = OuterProduct(transform.translation_ - enemy_->transform.translation_);
	ImGui::Text("%f", distance);
	ImGui::DragFloat3("L", &parts_[L_arm].rotation_.x, 0.1f);
	ImGui::DragFloat3("R", &parts_[R_arm].rotation_.x, 0.1f);
	//ImGui::DragFloat3("pas", &parts_[L_arm].rotation_.x, 0.1f);


	Move();
	Attack();
	

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
	
	models_[center]->Texture("Resources/sphere.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[Body]->Texture("Resources/player/body.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[Head]->Texture("Resources/player/head.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[L_arm]->Texture("Resources/player/limbs.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[R_arm]->Texture("Resources/player/limbs.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[L_leg]->Texture("Resources/player/limbs.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[R_leg]->Texture("Resources/player/limbs.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

}

void Player::Move()
{
	ImGui::DragFloat3("PlayerTranslate", &transform.translation_.x, 0.1f);
	Vector3 move = { 0.0f,0.0f,0.0f };
	const float speed = 0.2f;
	bool isMove = false;

	if (KeyInput::GetKey(DIK_W) && distance >= 15.0f)
	{
		move.z += speed;
		isMove = true;
	}
	if (KeyInput::GetKey(DIK_S))
	{
		move.z -= speed;
		isMove = true;
	}
	if (KeyInput::GetKey(DIK_D))
	{
		move.x += speed;
		isMove = true;
	}
	if (KeyInput::GetKey(DIK_A))
	{
		move.x -= speed;
		isMove = true;
	}


	//	仮腕回転アニメーション
	if (true)
	{
		isMove ? easeNum += 0.1f : easeNum -= 0.1f;
		easeNum = std::clamp(easeNum, 0.0f, 1.0f);
		float T = 1.0f - cosf((easeNum * std::numbers::pi_v<float>) / 2.0f);
		parts_[L_arm].rotation_.x = (1.0f - T) * 0.0f + (T * 0.6f);
		parts_[R_arm].rotation_.x = (1.0f - T) * 0.0f + (T * 0.6f);
		parts_[L_leg].rotation_.x = (1.0f - T) * 0.0f + (T * 0.6f);
		parts_[R_leg].rotation_.x = (1.0f - T) * 0.0f + (T * 0.6f);
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
		//transform.rotation_.y = camera->transform.rotation_.y;
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
	transform.translation_.x = std::clamp(transform.translation_.x, -70.0f, 70.0f);
	transform.translation_.z = std::clamp(transform.translation_.z, -70.0f, 70.0f);
	transform.translation_.y = 0.0f;
}

void Player::Attack()
{
	//	生存フラグが折れたら要素を取り除く
	if (std::erase_if(bullets_, [](std::unique_ptr<PlayerBullet>& bullet) {return bullet->isAlive == false; })) {
		enemy_->Damage();
	}
	if (KeyInput::PushKey(DIK_SPACE)) {
		if (distance >= 15.0f) {
		//	弾を追加する
			bullets_.push_back(std::make_unique<PlayerBullet>());
			(*bullets_.rbegin())->Initialize(enemy_->transform.translation_, transform);
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
		camera->transform.rotation_.y = atan2f(-pos.x, -pos.z);
		//transform.rotation_.x = atan2f(-position.y, -position.z);

		//	座標の反映
		camera->transform.translation_ = camera->position;
	}
}
