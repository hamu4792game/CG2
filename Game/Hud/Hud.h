#pragma once
#include "Engine/Texture/Texture2D.h"

class Hud
{
public:
	Hud() = default;
	~Hud() = default;

	void Initialize();

	void TextureLoad();

	void Draw(const Matrix4x4& viewProjection);
private:

	Texture2D enemyHp;
	Vector2 pos;
	Vector2 scale;
	float rotate;

};
