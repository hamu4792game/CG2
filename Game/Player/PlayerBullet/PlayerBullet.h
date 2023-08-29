#pragma once
#include "Engine/WorldTransform/WorldTransform.h"
#include "Engine/Texture/Model.h"

class PlayerBullet
{
public:
	PlayerBullet() = default;
	~PlayerBullet() = default;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="vector">対象 今回はenemy</param>
	/// <param name="player">発射位置</param>
	void Initialize(const Vector3& vector, const WorldTransform& player);

	void Update(const Vector3& vector);

	void Draw(const Matrix4x4& viewProjection, Model* model);
	//	生存フラグ
	bool isAlive = false;
	bool isHit = false;
	int time = 0;

private:
	WorldTransform transform;
	
	Vector3 move;

	//	速度
	Vector3 velocity;

};
