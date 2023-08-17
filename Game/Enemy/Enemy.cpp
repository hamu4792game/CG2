#include "Game/Enemy/Enemy.h"

void Enemy::Initialize()
{
	transform.translation_ = Vector3(0.0f, 10.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(1.0f, 1.0f, 1.0f);

	for (uint16_t i = 0; i < num; i++)
	{
		models_.push_back(std::make_unique<Model>());
	}

	parts_.resize(models_.size());
	parts_[0].parent_ = &transform;
	ModelLoad();
}

void Enemy::ModelLoad()
{
	models_[0]->Texture("Resources/float_Body.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Enemy::Update()
{
	transform.UpdateMatrix();
	for (auto& i : parts_) {
		i.UpdateMatrix();
	}
}

void Enemy::Draw(const Matrix4x4& viewProjection)
{
	for (uint16_t i = 0u; i < parts_.size(); i++)
	{
		models_[i]->Draw(parts_[i], viewProjection, 0xffffffff);
	}
}
