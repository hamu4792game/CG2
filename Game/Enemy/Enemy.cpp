#include "Game/Enemy/Enemy.h"
#include <algorithm>
#include "Engine/Easing/Easing.h"
#include "Game/Player/Player.h"

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

	transform.translation_ = Vector3(0.0f, 8.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(8.0f, 8.0f, 8.0f);

	parts_.resize(models_.size());
	parts_[PARTS::Egg].parent_ = &transform;
	parts_[PARTS::Body].parent_ = &transform;


	parts_[PARTS::Egg].scale_ = Vector3(1.1f, 1.1f, 1.1f);
	parts_[PARTS::Body].translation_.y = 0.0f;

	easeNum = 0.0f;
	isFlag = false;
	maxHp = 1000;
	hp = maxHp;
	power = 10;
	clearFlag = false;

	color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);

}

void Enemy::ModelLoad()
{
	models_[PARTS::Egg]->Texture("Resources/enemy/enemyType1.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	models_[PARTS::Body]->Texture("Resources/enemy/enemyBody.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Enemy::Update()
{
	//transform.translation_.x += 0.01f;

	switch (style)
	{
	case Enemy::STYLE::FIRSTChange:
		FirstUpdate();
		break;
	case Enemy::STYLE::SECOND:
		SecondUpdate();
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
		hp -= 100;
		hp = std::clamp(hp, 0, maxHp);
		color.w -= 15.0f;
		color.w = std::clamp(color.w, 0.0f, 255.0f);
		if (hp <= 0) {
			color.w = 255.0f;
			easeNum = 0.0f;
			style = STYLE::FIRSTChange;
		}
		break;
	case Enemy::STYLE::SECOND:
		hp -= 10;
		if (hp <= 0) {
			clearFlag = true;
		}
		break;
	case Enemy::STYLE::LAST:
		break;
	default:
		break;
	}
	
}

void Enemy::Change()
{
	if (isFlag) {
		easeStartPos = transform.translation_;
		easeEndPos = player_->GetWorldTransform().translation_;
		vec = FindVector(transform.translation_, player_->GetWorldTransform().translation_);
		transform.rotation_.y = std::atan2f(vec.x, vec.z);
		isFlag = false;
	}
	easeNum += 0.01f;
	easeNum = std::clamp(easeNum, 0.0f, 1.0f);
	if (easeNum != 1.0f) {
		float T = Easing::EaseInBack(easeNum);
		transform.translation_.x = (1.0f - T) * easeStartPos.x + T * easeEndPos.x;
		transform.translation_.z = (1.0f - T) * easeStartPos.z + T * easeEndPos.z;
	}
	else if (easeNum == 1.0f) {
		easeNum = 0.0f;
		pattern = PATTERN::Normal;
	}
	
	
}

void Enemy::FirstUpdate()
{
	easeNum == 1.0f && !isFlag ? isFlag = true, easeNum = 0.0f : 
	easeNum += 0.01f;
	hp += 10;
	hp = std::clamp(hp, 0, maxHp);
	if (!isFlag) {
		easeNum = std::clamp(easeNum, 0.0f, 1.0f);
		float T = Easing::EaseOutQuint(easeNum);
		transform.translation_.y = (1.0f - T) * 8.0f + T * 20.0f;
	}
	if (isFlag) {
		easeNum = std::clamp(easeNum, 0.0f, 1.0f);
		float T = Easing::EaseInBack(easeNum);
		transform.translation_.y = (1.0f - T) * 20.0f + T * 8.0f;
	}
	

	if (easeNum == 1.0f && isFlag) {
		easeNum = 0.0f;
		style = STYLE::SECOND;
	}
}

void Enemy::SecondUpdate()
{
	switch (pattern)
	{
	case PATTERN::Normal:
		transform.rotation_.y += AngleToRadian(1.0f);
		if (transform.rotation_.y >= AngleToRadian(180.0f)) {
			isFlag = true;
			pattern = PATTERN::Charge;
		}
		break;
	case PATTERN::Charge:
		Change();
		break;
	}
}

void Enemy::SetPlayer_ptr(Player* player)
{
	player_ = player;
}
