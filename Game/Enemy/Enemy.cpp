#include "Game/Enemy/Enemy.h"
#include <algorithm>
#include "Engine/Easing/Easing.h"
#include "externals/imgui/imgui.h"

Enemy::Enemy() : num(1)
{
	for (uint16_t i = 0; i < PARTS::Num; i++)
	{
		models_.push_back(std::make_unique<Model>());
	}
}

void Enemy::Initialize()
{
	style = STYLE::FIRST;

	transform.translation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(8.0f, 8.0f, 8.0f);

	parts_.resize(models_.size());
	parts_[PARTS::Egg].parent_ = &transform;
	parts_[PARTS::Body].parent_ = &transform;
	parts_[PARTS::Egg].translation_.y = 0.0f;
	parts_[PARTS::Body].translation_.y = -1.0f;

	easeNum = 0.0f;
	isFlag = false;

	color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);

}

void Enemy::ModelLoad()
{
	models_[PARTS::Egg]->Texture("Resources/enemy/enemyType1.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[PARTS::Body]->Texture("Resources/player/body.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Enemy::Update()
{
	//transform.translation_.x += 0.01f;

	ImGui::DragFloat3("enemy", &transform.translation_.x, 0.1f);

	switch (style)
	{
	case Enemy::STYLE::FIRSTChange:
		FirstUpdate();
		break;
	case Enemy::STYLE::SECOND:
		break;
	case Enemy::STYLE::LAST:
		break;
	}

	transform.UpdateMatrix();
	for (auto& i : parts_) {
		i.UpdateMatrix();
	}
}

void Enemy::Draw(const Matrix4x4& viewProjection)
{
	switch (style)
	{
	case Enemy::STYLE::FIRST:
		models_[PARTS::Body]->Draw(parts_[PARTS::Body], viewProjection, 0xffffffff);
		models_[PARTS::Egg]->Draw(parts_[PARTS::Egg], viewProjection, ChangeColor(color));
		break;
	case Enemy::STYLE::FIRSTChange:
		models_[PARTS::Body]->Draw(parts_[PARTS::Body], viewProjection, 0xffffffff);
		models_[PARTS::Egg]->Draw(parts_[PARTS::Egg], viewProjection, ChangeColor(color));
		break;
	case Enemy::STYLE::SECOND:
		models_[PARTS::Body]->Draw(parts_[PARTS::Body], viewProjection, 0xffffffff);
		break;
	case Enemy::STYLE::LAST:
		break;
	}
	
}

void Enemy::Damage()
{
	switch (style)
	{
	case Enemy::STYLE::FIRST:
		color.w -= 15.0f;
		color.w = std::clamp(color.w, 0.0f, 255.0f);
		if (color.w == 0.0f) {
			color.w = 255.0f;
			easeNum = 0.0f;
			style = STYLE::FIRSTChange;
		}
		break;
	case Enemy::STYLE::SECOND:
		break;
	case Enemy::STYLE::LAST:
		break;
	default:
		break;
	}
	
}

void Enemy::FirstUpdate()
{
	easeNum == 1.0f && !isFlag ? isFlag = true, easeNum = 0.0f : 
	easeNum += 0.01f;
	if (!isFlag) {
		easeNum = std::clamp(easeNum, 0.0f, 1.0f);
		float T = Easing::EaseOutQuint(easeNum);
		transform.translation_.y = (1.0f - T) * 10.0f + T * 20.0f;
	}
	if (isFlag) {
		easeNum = std::clamp(easeNum, 0.0f, 1.0f);
		float T = Easing::EaseInBack(easeNum);
		transform.translation_.y = (1.0f - T) * 20.0f + T * 5.0f;
	}
	

	if (easeNum == 1.0f && isFlag) {
		easeNum = 0.0f;
		style = STYLE::SECOND;
	}
}
