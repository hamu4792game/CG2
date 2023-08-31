#pragma once
#include "Engine/Texture/Texture2D.h"
#include "Game/Player/Player.h"

class Hud
{
public:
	Hud() = default;
	~Hud() = default;

	void Initialize();

	void TextureLoad();

	void Update();

	void Draw(const Matrix4x4& viewProjection);
private:

	Texture2D playerHp;
	Texture2D enemyHp;

	Vector2 pos;
	Vector2 playerscale;
	Vector2 enemypos;
	Vector2 enemyscale;
	float rotate;

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;

public:
	void SetPlayer_ptr(Player* player) { player_ = player; };
	void SetEnemy_ptr(Enemy* enemy) { enemy_ = enemy; };

};
