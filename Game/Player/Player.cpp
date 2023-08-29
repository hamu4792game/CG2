#include "Player.h"
#include "externals/imgui/imgui.h"
#include "math/Vector3.h"
#include "math/Matrix4x4.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include <cmath>
#include <algorithm>
#include <numbers>
#include "Engine/Easing/Easing.h"

Player::Player() : num(20)
{
	for (uint16_t i = 0u; i < PARTS::Num; i++) {
		models_.push_back(std::make_unique<Model>());
	}
	for (uint16_t i = 0u; i < num; i++) {
		bulletModel_[i] = std::make_unique<Model>();
		bullets_[i] = std::make_unique<PlayerBullet>();
	}
}

void Player::Initialize()
{
	transform.translation_ = Vector3(0.0f, 0.0f, -40.0f);

	parts_.resize(models_.size());
	for (auto& i : parts_) {
		i.parent_ = &parts_[Body];
	}
	parts_[center].parent_ = &transform;
	parts_[Body].parent_ = &transform;
	parts_[Weapon].parent_ = &transform;

	parts_[Body].translation_ = { 0.0f,2.0f,0.0f };

	parts_[Head].translation_ = { 0.0f,2.1f,0.0f };
	parts_[L_arm].translation_ = { 1.3f,1.5f,0.0f };
	parts_[R_arm].translation_ = { -1.3f,1.5f,0.0f };
	parts_[L_leg].translation_ = { 0.5f,0.0f,0.0f };
	parts_[R_leg].translation_ = { -0.5f,0.0f,0.0f };
	parts_[Weapon].translation_ = Vector3(-3.0f, 5.0f, 0.0f);
	parts_[Weapon].scale_ = { 0.5f,0.5f,0.5f };

	color = 0xffffffff;
	distance = 0.0f;

	easeNum = 0.0f;
	jampflg = false;
	velocity = 0.0f;

	for (uint16_t i = 0u; i < num; i++) {
		bullets_[i]->isAlive = false;
	}

	//	カメラの初期角度の設定
	camera->transform.rotation_.x = 0.08f;
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
	Jamp();
	//	移動制限
	MoveLimit();
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
	models_[Weapon]->Texture("Resources/enemy/enemyType1.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

	for (uint16_t i = 0u; i < num; i++) {
		bulletModel_[i]->Texture("Resources/bullet/bullet.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	}

}

void Player::Move()
{
	ImGui::DragFloat3("PlayerTranslate", &parts_[Weapon].translation_.x, 0.1f);
	Vector3 move = { 0.0f,0.0f,0.0f };
	float speed = 0.2f;
	bool isMove = false;
	//	ダッシュ（はやい）
	if (KeyInput::GetKey(DIK_LSHIFT))
	{
		speed = 0.4f;
	}

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
	//	padが接続されているなら
	if (KeyInput::GetInstance()->GetPadConnect()) {
		Vector2 pMove(0.0f, 0.0f);
		pMove = KeyInput::GetInstance()->GetPadLStick();
		//	移動があれば代入する
		if (pMove.x != 0.0f || pMove.y != 0.0f)
		{
			move.x = pMove.x;
			move.z = pMove.y;
			isMove = true;
		}
	}

	//	ジャンプの初速
	if ((KeyInput::PushKey(DIK_B) || KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) && !jampflg)
	{
		velocity = 2.0f;
		jampflg = true;
	}

	//	仮腕回転アニメーション
	if (true)
	{
		isMove ? easeNum += 0.1f : easeNum -= 0.1f;
		easeNum = std::clamp(easeNum, 0.0f, 1.0f);
		float T = Easing::EaseInSine(easeNum);
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
		//transform.rotation_.y = std::atan2f(move.x, move.z);
		parts_[Body].rotation_.y = std::atan2f(move.x, move.z);
		//	カメラの回転分を引くことで合わせる
		parts_[Body].rotation_.y -= camera->transform.rotation_.y;

		//	敵の方向を見続ける
		//transform.rotation_.y = camera->transform.rotation_.y;


		/*parts_[Weapon].translation_ = oldPos[0];
		for (uint16_t i = 0u; i < 9; i++) {
			oldPos[i] = oldPos[i + 1];
		}
		oldPos[9] = transform.translation_ + move;*/

		move.y = 0.0f;

	}
	//	座標移動（ベクトルの加算）
	if (fabs(transform.translation_.x + move.x) <= 1.0f && fabs(transform.translation_.z + move.z) <= 1.0f) {
		transform.translation_ += oldMove;
	}
	else {
		transform.translation_ += move;
		oldMove = move;
	}

	//	回転を合わせる
	parts_[Weapon].rotation_.y = camera->transform.rotation_.y;
	transform.rotation_.y = camera->transform.rotation_.y;
	

	//	みずかみ式魔法の数学　よくわからん
	//	武器の座標移動、特別枠
	//	カメラを回転してあげる 逆ベクトルなので-
	/*float ro = atan2f(enemy_->transform.translation_.x - transform.translation_.x, enemy_->transform.translation_.z - transform.translation_.z);

	float dis = sqrtf(powf(enemy_->transform.translation_.x - transform.translation_.x, 2.0f) + powf(enemy_->transform.translation_.z - transform.translation_.z, 2.0f));
	float theta = atanf(3.5f / dis);
	float a = dis / 40.0f;
	float b = sqrtf(1612.25f);
	float c = theta + ro;
	Vector3 ve = Vector3(0.0f, parts_[Weapon].translation_.y, -b * a);
	Matrix4x4 mat = MakeRotateYMatrix(c);
	parts_[Weapon].translation_ = Transform(ve, mat);
	parts_[Weapon].translation_.y = transform.translation_.y + 5.0f;
	parts_[Weapon].translation_.x += enemy_->transform.translation_.x;
	parts_[Weapon].translation_.z += enemy_->transform.translation_.z;*/


}

void Player::Draw(const Matrix4x4& viewProjection)
{
	for (uint16_t i = 0u; i < parts_.size(); i++)
	{
		models_[i]->Draw(parts_[i], viewProjection, color);
	}
	for (uint16_t i = 0u; i < num; i++) {
		bullets_[i]->Draw(viewProjection, bulletModel_[i].get());
	}
}

void Player::MoveLimit()
{
	transform.translation_.x = std::clamp(transform.translation_.x, -70.0f, 70.0f);
	transform.translation_.y = std::clamp(transform.translation_.y, 0.0f, 50.0f);
	transform.translation_.z = std::clamp(transform.translation_.z, -70.0f, 70.0f);
}

void Player::Attack()
{
	//	弾が消えたらダメージ処理を行う
	for (uint16_t i = 0u; i < num; i++) {
		if (bullets_[i]->isHit == true) {
			enemy_->Damage();
			break;
		}
	}

	//	生存フラグが折れたら要素を取り除く
	if (KeyInput::PushKey(DIK_SPACE) || KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		if (distance >= 15.0f) {
		//	弾を追加する
			for (uint16_t i = 0u; i < num; i++) {
				if (bullets_[i]->isAlive == false) {
					bullets_[i]->Initialize(enemy_->transform.translation_, parts_[Weapon]);
					break;
				}
			}
		}
	}
}

void Player::Jamp()
{
	const float gravity = 0.1f;
	if (jampflg) {
		velocity -= gravity;
		transform.translation_.y += velocity;
	}
	if (transform.translation_.y <= 0.0f) {
		jampflg = false;
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
