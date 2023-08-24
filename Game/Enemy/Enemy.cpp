#include "Game/Enemy/Enemy.h"
#include <algorithm>

void Enemy::Initialize()
{
	transform.translation_ = Vector3(0.0f, 10.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(10.0f, 10.0f, 10.0f);

	for (uint16_t i = 0; i < num; i++)
	{
		models_.push_back(std::make_unique<Model>());
	}

	parts_.resize(models_.size());
	parts_[0].parent_ = &transform;
	parts_[0].translation_.y = -1.0f;

	color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);

	ModelLoad();
}

void Enemy::ModelLoad()
{
	models_[0]->Texture("Resources/enemy/enemy.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Enemy::Update()
{
	transform.translation_.x += 0.01f;

	transform.UpdateMatrix();
	for (auto& i : parts_) {
		i.UpdateMatrix();
	}
}

void Enemy::Draw(const Matrix4x4& viewProjection)
{
	for (uint16_t i = 0u; i < parts_.size(); i++)
	{
		models_[i]->Draw(parts_[i], viewProjection, ChangeColor(color));
	}
}

void Enemy::Damage()
{
	color.w -= 15.0f;
	color.w = std::clamp(color.w, 0.0f, 255.0f);
}
