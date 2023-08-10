#include "Player.h"
#include "externals/imgui/imgui.h"
#include "math/Vector3.h"
#include "math/Matrix4x4.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include <cmath>

void Player::Initialize()
{
	transform.translation_ = Vector3(0.0f, 0.0f, 0.0f);

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

void Player::ModelLoad()
{
	
	models_[0]->Texture("Resources/float_Body.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[1]->Texture("Resources/float_Head.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[2]->Texture("Resources/float_L_arm.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[3]->Texture("Resources/float_R_arm.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");

}

void Player::Update()
{
	Vector3 move = { 0.0f,0.0f,0.0f };

	if (KeyInput::GetKey(DIK_W))
	{
		move.z += 0.2f;
	}
	if (KeyInput::GetKey(DIK_S))
	{
		move.z -= 0.2f;
	}
	if (KeyInput::GetKey(DIK_D))
	{
		move.x += 0.2f;
	}
	if (KeyInput::GetKey(DIK_A))
	{
		move.x -= 0.2f;
	}

	//	移動があれば更新
	if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f)
	{
		//	移動量の正規化
		move = Normalize(move) * 0.2f;
		//	移動ベクトルをカメラの角度だけ回転させる
		move = TransformNormal(move, MakeRotateMatrix(camera->rotate));

		//	移動方向に見た目を合わせる
		transform.rotation_.y = std::atan2f(move.x, move.z);
	}
	//	座標移動（ベクトルの加算）
	transform.translation_ += move;

	transform.UpdateMatrix();
	for (auto& i : parts_) {
		i.UpdateMatrix();
	}
}

void Player::Draw(const Matrix4x4& viewProjection)
{
	for (uint16_t i = 0u; i < parts_.size(); i++)
	{
		models_[i]->Draw(parts_[i], viewProjection, color);
	}
}
