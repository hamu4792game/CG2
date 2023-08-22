#pragma once
#include "Engine/WorldTransform/WorldTransform.h"
#include "Engine/Texture/Model.h"

class Player;

class PlayerBullet
{
public:
	PlayerBullet() = default;
	~PlayerBullet() = default;

	void Initialize(const Vector3& camerarotate);
	//	モデルのロード
	void ModelLoad();

	void Update();

	void Draw(const Matrix4x4& viewProjection);

private:
	WorldTransform transform;
	
	Player* player;

	Vector3 move;

	//	Y軸の角度
	float degree;

	//	速度
	Vector3 velocity;

	//	モデルデータ配列
	std::unique_ptr <Model> models_;


};
