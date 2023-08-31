#include "Hud.h"

void Hud::Initialize()
{
	pos = Vector2(315.0f, -290.0f);
	playerscale = Vector2(5.0f, 3.0f);
	enemypos = Vector2(0.0f, 290.0f);
	enemyscale = Vector2(9.0f, 3.0f);
	rotate = 0.0f;
}

void Hud::TextureLoad()
{
	playerHp.Texture("Resources/hud/hpber.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	enemyHp.Texture("Resources/hud/hpber.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Hud::Update()
{
	playerscale.x = (static_cast<float>(player_->hp) / static_cast<float>(player_->maxHp)) * 5.0f;
	enemyscale.x = (static_cast<float>(enemy_->hp) / static_cast<float>(enemy_->maxHp)) * 9.0f;

	pos.x = (static_cast<float>(player_->hp) / static_cast<float>(player_->maxHp)) * 315.0f;
}

void Hud::Draw(const Matrix4x4& viewProjection)
{
	playerHp.Draw(pos, playerscale, 0.0f, viewProjection, 0x00ff00ff);
	enemyHp.Draw(enemypos, enemyscale, 0.0f, viewProjection, 0xff0000ff);
}
