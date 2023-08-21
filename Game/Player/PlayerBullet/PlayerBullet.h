#pragma once
#include "Engine/WorldTransform/WorldTransform.h"

class Player;

class PlayerBullet
{
public:
	PlayerBullet() = default;
	~PlayerBullet() = default;

	void Initialize();

	void Update();

	void Draw();

private:
	WorldTransform transform;
	
	Player* player;

	//	Y軸の角度
	float degree;

	//	速度
	Vector3 velocity;



};
